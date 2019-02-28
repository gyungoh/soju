#ifndef __HAN_H
#define __HAN_H

#define KEY_NONE -1
#define ARR_SIZE(a) (sizeof(a) / sizeof(a[0]))

struct conv_tbl
{
	short org;
	short begin;
	unsigned int input_mask;
};

static const short key_cho[] = 
{ 6, -1, 14, 11, 3, 5, 18, -1, -1, -1, -1, -1, -1, -1, -1, -1, 7, 0, 2, 9, -1, 17, 12, 16, -1, 15 };

static const short key_cho_sh[] = 
{ 6, -1, 14, 11, 4, 5, 18, -1, -1, -1, -1, -1, -1, -1, -1, -1, 8, 1, 2, 10, -1, 17, 13, 16, -1, 15 };

static const short key_jung[] =
{ -1, 17, -1, -1, -1, -1, -1, 8, 2, 4, 0, 20, 18, 13, 1, 5, -1, -1, -1, -1, 6, -1, -1, -1, 12, -1 };

static const short key_jung_sh[] =
{ -1, 17, -1, -1, -1, -1, -1, 8, 2, 4, 0, 20, 18, 13, 3, 7, -1, -1, -1, -1, 6, -1, -1, -1, 12, -1 };

static const int key_jong[] =
{ 16, -1, 23, 21, 7, 8, 27, -1, -1, -1, -1, -1, -1, -1, -1, -1, 17, 1, 4, 19, -1, 26, 22, 25, -1, 24 };

static const short key_jong_sh[] =
{ 16, -1, 23, 21, 7, 8, 27, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 2, 4, 20, -1, 26, -1, 25, -1, 24 };

static const short jong_to_cho[] =
{ -1, 0, 1, 9, 2, 12, 18, 3, 5, 0, 6, 7, 9, 16, 17, 18, 6, 7, 9, 9, 10, 11, 12, 14, 15, 16, 17, 18 };

static const short jong_bs[] =
{ 0, 0, 0, 1, 0, 4, 4, 0, 0, 8, 8, 8, 8, 8, 8, 8, 0, 0, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static const short jung_bs[] =
{ -1, -1, -1, -1, -1, -1, -1, -1, -1, 8, 8, 8, -1, -1, 13, 13, 13, -1, -1, 18, -1 };

static const struct conv_tbl comb_jung[] =
{
	{8, 9, (1 << 0) | (1 << 1) | (1 << 20)},
	{13, 14, (1 << 4) | (1 << 5) | (1 << 20)},
	{18, 19, (1 << 20)}
};

static const struct conv_tbl comb_jong[] =
{
	{1, 3, (1 << 19)},
	{4, 5, (1 << 22) | (1 << 27)},
	{8, 9, (1 << 1) | (1 << 16) | (1 << 17) | (1 << 19) | (1 << 25) | (1 << 26) | (1 << 27)},
	{17, 18, (1 << 19)}
};

static enum { CHO, JUNG, JONG } state;
static short cho = -1, jung = -1, jong = 0;

static short conv(const struct conv_tbl *tbl, int tbl_len, int org, int input)
{
	unsigned int mask;

	while (tbl_len--)
		if (tbl[tbl_len].org == org)
		{
			mask = tbl[tbl_len].input_mask;
			if (mask & (1 << input))
				return tbl[tbl_len].begin + __builtin_popcount(mask & ((1 << input) - 1));
			break;
		}

	return -1;
}

static unsigned short make_code(void)
{
	static const unsigned short cho_unicode[] =
	{ 1, 2, 4, 7, 8, 9, 17, 18, 19, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30 };

	if (cho >= 0 && jung >= 0)
		return 0xAC00 + cho * (21 * 28) + jung * 28 + jong;
	else if (cho >= 0)
		return 0x3130 + cho_unicode[cho];
	else if (jung >= 0)
		return 0x314F + jung;

	return 0;
}

static unsigned short han_process(signed char key)
{
	int shift;
	short c;
	unsigned short ret = 0;

	if (key >= 'a')
	{
		key -= 'a';
		shift = 0;
	}
	else
	{
		key -= 'A';
		shift = 1;
	}

	while (key >= 0)
		switch(state)
		{
		case CHO:
			c = shift ? key_cho_sh[key] : key_cho[key];
			if (c >= 0)
			{
				if (cho == -1)
				{
					cho = c;
					key = KEY_NONE;
				}
				else
				{
					ret = make_code();
					cho = -1;
				}
			}
			else
				state = JUNG;
			break;

		case JUNG:
			c = shift ? key_jung_sh[key] : key_jung[key];
			if (c >= 0 && (jung == -1 || (c = conv(comb_jung, ARR_SIZE(comb_jung), jung, c)) >= 0))
			{
				jung = c;
				key = KEY_NONE;
			}
			else if (cho == -1)
			{
				ret = make_code();
				jung = -1;
				state = CHO;
			}
			else
				state = JONG;
			break;

		case JONG:
			c = shift ? key_jong_sh[key] : key_jong[key];
			if (c >= 0 && (jong == 0 || (c = conv(comb_jong, ARR_SIZE(comb_jong), jong, c)) >= 0))
			{
				jong = c;
				key = KEY_NONE;
			}
			else
			{
				if (key_jung[key] >= 0)
				{
					c = jong_to_cho[jong];
					jong = jong_bs[jong];
				}
				else
					c = -1;

				ret = make_code();
				cho = c;
				jung = -1;
				jong = 0;
				state = CHO;
			}
			break;
		}

	return ret;
}

static unsigned short han_preedit(void)
{
	return make_code();
}

static int han_is_empty(void)
{
	return cho == -1 && jung == -1 && jong == 0;
}

static unsigned short han_flush(void)
{
	unsigned short ret = make_code();

	state = CHO;
	cho = -1;
	jung = -1;
	jong = 0;

	return ret;
}

static int han_backspace(void)
{
	if (jong > 0)
		jong = jong_bs[jong];
	else if (jung >= 0)
		jung = jung_bs[jung];
	else if (cho >= 0)
		cho = -1;
	else
		return 0;

	return 1;
}

#endif