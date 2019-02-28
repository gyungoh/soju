#include <X11/keysym.h>
#include <X11/X.h>
#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
#include <xcb/xcb_keysyms.h>
#include <imdkit.h>
#include "han.h"

#define COMP_LEN 9

static uint32_t style = XCB_IM_PreeditCallbacks | XCB_IM_StatusNothing;
static const xcb_im_styles_t styles = {1, &style};

static char *encoding = "COMPOUND_TEXT";
static const xcb_im_encodings_t encodings = {1, &encoding};

static xcb_im_ximtriggerkey_fr_t key = {XK_Hangul, 0, 0};
static const xcb_im_trigger_keys_t keys = {1, &key};

static int is_preed = 0;
static int preed_len;

static inline char *ucs_to_compound_text(const unsigned short ucs)
{
	static char ret[] = {0x1B, 0x25, 0x47, 0x00, 0x00, 0x00, 0x1B, 0x25, 0x40, 0x00};

	ret[3] = 0xE0 | (ucs >> 12);
	ret[4] = 0x80 | ((ucs >> 6) & 0x3F);
	ret[5] = 0x80 | (ucs & 0x3F);

	return ret;
}

static inline void flush(xcb_im_t *im, xcb_im_input_context_t *ic)
{
	if (is_preed)
	{
		xcb_im_preedit_done_callback(im, ic);
		is_preed = 0;
		preed_len = 0;
	}

	if (!han_is_empty())
		xcb_im_commit_string(im, ic, XCB_XIM_LOOKUP_CHARS,
				ucs_to_compound_text(han_flush()), COMP_LEN, 0);
}

static void callback(xcb_im_t *im, xcb_im_client_t *client,
		xcb_im_input_context_t *ic, const xcb_im_packet_header_fr_t *hdr,
		void *frame, void *arg, void *user_data)
{
	static uint32_t feedback[] = {1};
	int processed = 0;
	xcb_key_press_event_t *e = arg;
	xcb_keysym_t sym;
	xcb_im_preedit_draw_fr_t preed;
	unsigned short ucs;

	switch(hdr->major_opcode)
	{
	case XCB_XIM_TRIGGER_NOTIFY:
	case XCB_XIM_UNSET_IC_FOCUS:
		flush(im, ic);
		break;;

	case XCB_XIM_FORWARD_EVENT:
		sym = xcb_key_symbols_get_keysym(user_data, e->detail, 0);
		if (((sym >= XK_A && sym <= XK_Z) || (sym >= XK_a && sym <= XK_z)) &&
			!(e->state & (ControlMask | Mod1Mask | Mod3Mask | Mod4Mask | Mod5Mask)))
		{
			if (e->state & ShiftMask)
				sym = sym >= XK_a && sym <= XK_z ? sym - (XK_a - XK_A) : sym;
			else
				sym = sym >= XK_A && sym <= XK_Z ? sym + (XK_a - XK_A) : sym;

			xcb_im_commit_string(im, ic, XCB_XIM_LOOKUP_CHARS,
					ucs_to_compound_text(han_process(sym)), COMP_LEN, 0);
			processed = 1;
		}
		else if (sym == XK_BackSpace)
		{
			processed = han_backspace();
		}
		else if (sym == XK_Shift_L || sym == XK_Shift_R)
			break;

		if (processed)
		{
			if ((ucs = han_preedit()))
			{
				if (!is_preed)
				{
					xcb_im_preedit_start_callback(im, ic);	
					is_preed = 1;
					preed_len = 0;
				}

				preed.caret = 1;
				preed.chg_first  = 0;
				preed.chg_length = preed_len;
				preed.status = 0;
				preed.length_of_preedit_string = COMP_LEN;
				preed.preedit_string = ucs_to_compound_text(ucs);
				preed.feedback_array.size = ARR_SIZE(feedback);
				preed.feedback_array.items = feedback;
				xcb_im_preedit_draw_callback(im, ic, &preed);
				preed_len = 1;
			}
			else if (is_preed)
			{
				xcb_im_preedit_done_callback(im, ic);
				is_preed = 0;
			}
		}
		else
		{
			flush(im, ic);
			xcb_im_forward_event(im, ic, e);
		}
		break;
	}
}

int main(int argc, char* argv[])
{
	xcb_generic_event_t *e;
	int s_no;
	xcb_im_t *im;
	xcb_connection_t *c = xcb_connect(NULL, &s_no);
	xcb_screen_t *s = xcb_aux_get_screen(c, s_no);
	xcb_key_symbols_t *symbols = xcb_key_symbols_alloc(c);
	xcb_window_t w = xcb_generate_id(c);

	xcb_create_window(c, XCB_COPY_FROM_PARENT, w, s->root, 0, 0, 1, 1, 0,
			XCB_WINDOW_CLASS_INPUT_OUTPUT, s->root_visual, 0, NULL);

	if (!(im = xcb_im_create(c, s_no, w, "soju", XCB_IM_ALL_LOCALES,
			&styles, &keys, &keys, &encodings, 0, callback, symbols)))
		return 1;
	xcb_im_open_im(im);

	while ((e = xcb_wait_for_event(c)))
	{
		xcb_im_filter_event(im, e);
		free(e);
	}

	xcb_im_close_im(im);
	xcb_im_destroy(im);
	xcb_key_symbols_free(symbols);
	xcb_disconnect(c);
	return 0;
}