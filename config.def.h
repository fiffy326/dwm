/* See LICENSE file for copyright and license details. */

/* appearance */
static unsigned int borderpx   = 1;  /* border pixel of windows */
static unsigned int snap       = 32; /* snap pixel */
static int showbar             = 1;  /* 0 means no bar */
static int topbar              = 1;  /* 0 means bottom bar */
static int smartgaps           = 0;  /* 1 means no outer gap when only 1 window */
static unsigned int gappih     = 20; /* horiz inner gap between windows */
static unsigned int gappiv     = 10; /* vert inner gap between windows */
static unsigned int gappoh     = 10; /* vert outer gap around windows */
static unsigned int gappov     = 30; /* horiz outer gap around windows */
static int swallowfloating     = 0;  /* 1 means swallow floating windows by default */
static int viewonrulestag      = 1;  /* 1 means opened apps move view to rule tag */
static int user_bh             = 2;  /* 2 is default spacing around bar font */
static int showtitle           = 1;  /* 1 means show the window title in the bar */
static int focusonwheel        = 0;
static const char *fonts[]     = { "monospace:size=10" };
static char dmenufont[]        = "monospace:size=10";
static char col_norm_fg[]      = "#bbbbbb";
static char col_norm_bg[]      = "#222222";
static char col_norm_border[]  = "#444444";
static char col_sel_fg[]       = "#eeeeee";
static char col_sel_bg[]       = "#005577";
static char col_sel_border[]   = "#005577";
static char col_urg_fg[]       = "#eeeeee";
static char col_urg_bg[]       = "#005577";
static char col_urg_border[]   = "#ff0000";
static char col_title_fg[]     = "#eeeeee";
static char col_title_bg[]     = "#005577";
static char col_title_border[] = "#005577";
static char col_dmenu_hover[]  = "#005577";
static char *colors[][3]       = {
	/*                fg            bg            border           */
	[SchemeNorm]  = { col_norm_fg,  col_norm_bg,  col_norm_border  },
	[SchemeSel]   = { col_sel_fg,   col_sel_bg,   col_sel_border   },
	[SchemeUrg]   = { col_urg_fg,   col_urg_bg,   col_urg_border   },
	[SchemeTitle] = { col_title_fg, col_title_bg, col_title_border },
};
static const unsigned int baralpha    = 0xd0;
static const unsigned int borderalpha = OPAQUE;
static const unsigned int alphas[][3] = {
	/*                fg      bg        border      */
	[SchemeNorm]  = { OPAQUE, baralpha, borderalpha },
	[SchemeSel]   = { OPAQUE, baralpha, borderalpha },
	[SchemeUrg]   = { OPAQUE, baralpha, borderalpha },
	[SchemeTitle] = { OPAQUE, baralpha, borderalpha },
};

/* tagging */
static const int momentaryalttags = 0; /* 1 means alttags will show only when key is held down*/
static const char *tags[]    = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
static const char *tagsalt[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
static const char *defaulttagapps[] = { "st", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

static unsigned int ulinepad	= 5;	/* horizontal padding between the underline and tag */
static unsigned int ulinestroke	= 2;	/* thickness / height of the underline */
static unsigned int ulinevoffset	= 0;	/* how far above the bottom of the bar the line should appear */
static int ulineall 		= 0;	/* 1 to show underline on all tags, 0 for just the active ones */

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class     instance  title           tags mask  isfloating  isterminal  noswallow  monitor */
	{ "Gimp",    NULL,     NULL,           0,         1,          0,           0,        -1 },
	{ "Firefox", NULL,     NULL,           1 << 8,    0,          0,          -1,        -1 },
	{ "St",      NULL,     NULL,           0,         0,          1,           0,        -1 },
	{ NULL,      NULL,     "Event Tester", 0,         0,          0,           1,        -1 }, /* xev */
};

/* layout(s) */
static float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "[M]",      monocle },
	{ "[@]",      spiral },
	{ "[\\]",     dwindle },
	{ "H[]",      deck },
	{ "TTT",      bstack },
	{ "===",      bstackhoriz },
	{ "HHH",      grid },
	{ "###",      nrowgrid },
	{ "---",      horizgrid },
	{ ":::",      gaplessgrid },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ NULL,       NULL },
};

/* custom symbols for nr. of clients in monocle layout */
/* when clients >= LENGTH(monocles), uses the last element */
static const char *monocles[] = { "[1]", "[2]", "[3]", "[4]", "[5]", "[6]", "[7]", "[8]", "[9]", "[9+]" };

/* key definitions */
#define MODKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

#define STATUSBAR "dwmblocks"

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_norm_bg, "-nf", col_norm_fg, "-sb", col_dmenu_hover, "-sf", col_sel_fg, NULL };
#define TERMINAL_ENVVAR "TERMINAL"

/* Xresources preferences to load at startup */
ResourcePref resources[] = {
	{ "borderpx",         INTEGER, &borderpx         },
	{ "snap",             INTEGER, &snap             },
	{ "showbar",          INTEGER, &showbar          },
	{ "topbar",           INTEGER, &topbar           },
	{ "smartgaps",        INTEGER, &smartgaps        },
	{ "gappih",           INTEGER, &gappih           },
	{ "gappiv",           INTEGER, &gappiv           },
	{ "gappoh",           INTEGER, &gappoh           },
	{ "gappov",           INTEGER, &gappov           },
	{ "swallowfloating",  INTEGER, &swallowfloating  },
	{ "viewonrulestag",   INTEGER, &viewonrulestag   },
	{ "user_bh",          INTEGER, &user_bh          },
	{ "showtitle",        INTEGER, &showtitle        },
	{ "col_norm_fg",      STRING,  &col_norm_fg      },
	{ "col_norm_bg",      STRING,  &col_norm_bg      },
	{ "col_norm_border",  STRING,  &col_norm_border  },
	{ "col_sel_fg",       STRING,  &col_sel_fg       },
	{ "col_sel_bg",       STRING,  &col_sel_bg       },
	{ "col_sel_border",   STRING,  &col_sel_border   },
	{ "col_urg_fg",       STRING,  &col_urg_fg       },
	{ "col_urg_bg",       STRING,  &col_urg_bg       },
	{ "col_urg_border",   STRING,  &col_urg_border   },
	{ "col_title_fg",     STRING,  &col_title_fg     },
	{ "col_title_bg",     STRING,  &col_title_bg     },
	{ "col_title_border", STRING,  &col_title_border },
	{ "col_dmenu_hover",  STRING,  &col_dmenu_hover  },
	{ "ulinepad",         INTEGER, &ulinepad         },
	{ "ulinestroke",      INTEGER, &ulinestroke      },
	{ "ulinevoffset",     INTEGER, &ulinevoffset     },
	{ "ulineall",         INTEGER, &ulineall         },
	{ "mfact",            FLOAT,   &mfact            },
	{ "nmaster",          INTEGER, &nmaster          },
	{ "resizehints",      INTEGER, &resizehints      },
	{ "lockfullscreen",   INTEGER, &lockfullscreen   },
};

static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_s,      spawndefault,   {0} },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_h,      setcfact,       {.f = +0.25} },
	{ MODKEY|ShiftMask,             XK_l,      setcfact,       {.f = -0.25} },
	{ MODKEY|ShiftMask,             XK_o,      setcfact,       {.f =  0.00} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY|Mod4Mask,              XK_u,      incrgaps,       {.i = +1 } },
	{ MODKEY|Mod4Mask|ShiftMask,    XK_u,      incrgaps,       {.i = -1 } },
	{ MODKEY|Mod4Mask,              XK_i,      incrigaps,      {.i = +1 } },
	{ MODKEY|Mod4Mask|ShiftMask,    XK_i,      incrigaps,      {.i = -1 } },
	{ MODKEY|Mod4Mask,              XK_o,      incrogaps,      {.i = +1 } },
	{ MODKEY|Mod4Mask|ShiftMask,    XK_o,      incrogaps,      {.i = -1 } },
	{ MODKEY|Mod4Mask,              XK_6,      incrihgaps,     {.i = +1 } },
	{ MODKEY|Mod4Mask|ShiftMask,    XK_6,      incrihgaps,     {.i = -1 } },
	{ MODKEY|Mod4Mask,              XK_7,      incrivgaps,     {.i = +1 } },
	{ MODKEY|Mod4Mask|ShiftMask,    XK_7,      incrivgaps,     {.i = -1 } },
	{ MODKEY|Mod4Mask,              XK_8,      incrohgaps,     {.i = +1 } },
	{ MODKEY|Mod4Mask|ShiftMask,    XK_8,      incrohgaps,     {.i = -1 } },
	{ MODKEY|Mod4Mask,              XK_9,      incrovgaps,     {.i = +1 } },
	{ MODKEY|Mod4Mask|ShiftMask,    XK_9,      incrovgaps,     {.i = -1 } },
	{ MODKEY|Mod4Mask,              XK_0,      togglegaps,     {0} },
	{ MODKEY|Mod4Mask|ShiftMask,    XK_0,      defaultgaps,    {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY,                       XK_n,      togglealttag,   {0} },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button1,        sigstatusbar,   {.i = 1} },
	{ ClkStatusText,        0,              Button2,        sigstatusbar,   {.i = 2} },
	{ ClkStatusText,        0,              Button3,        sigstatusbar,   {.i = 3} },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
