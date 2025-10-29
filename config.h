/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx = 2;
static const unsigned int snap = 32;
static const int showbar = 1;
static const int topbar = 1;
static const char *fonts[] = {
    "FiraCode Nerd Font:size=13:antialias=true:autohint=true", // main font
    "Noto Color Emoji:size=10" // emoji (match main font size)
};

static const char dmenufont[] = "FiraCode Nerd Font:size=15";
static const char col_bg[] = "#0a0a0f"; // Deep black-blue background
static const char col_border_unf[] =
    "#5500ff"; // Unfocused window border (neon violet)
static const char col_text[] = "#00fff7";   // Primary text (neon cyan)
static const char col_bar[] = "#0d0d18";    // Bar background
static const char col_accent[] = "#ff0080"; // Accent (pink highlight)
static const char col_border_sel[] =
    "#00ffff"; // Focused window border (electric cyan)

static const char *colors[][3] = {
    /*               fg         bg         border   */
    [SchemeNorm] = {col_text, col_bg, col_border_unf},
    [SchemeSel] = {col_accent, col_bar, col_border_sel},
};
/* tagging */
static const char *tags[] = {"", "󰞷 ", "3", "4", "5", "6", "7", "8", "9"};

static const Rule rules[] = {
    /* xprop(1):
     *	WM_CLASS(STRING) = instance, class
     *	WM_NAME(STRING) = title
     */
    /* class      instance    title       tags mask     isfloating   monitor */
    {"Gimp", NULL, NULL, 0, 1, -1},
    {"kitty", "ff_term", NULL, 1 << 0, 1, -1},
    {"kitty", "rsclock_term", NULL, 1 << 0, 1, -1},
    {"kitty", "ncmpcpp_term", NULL, 1 << 0, 1, -1},
    {"kitty", "htop_term", NULL, 1 << 0, 1, -1},
    {"kitty", "cava_term", NULL, 1 << 0, 1, -1},

    {"KittyWS2", NULL, NULL, 1 << 1, 0, -1},
    {"Chromium-browser", NULL, NULL, 1 << 2, 0, -1}, /* workspace 3 */
    /* Kitty with tmux in workspace 9 */
    {"KittyTmux", NULL, NULL, 1 << 8, 0, -1},

};

/* layout(s) */
static const float mfact = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster = 1;    /* number of clients in master area */
static const int resizehints =
    1; /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen =
    1; /* 1 will force focus on the fullscreen window */
static const int refreshrate =
    120; /* refresh rate (per second) for client move/resize */
/* forward declaration for layouts */
void tile(Monitor *m);

static const Layout layouts[] = {
    /* symbol     arrange function */
    {"[]=", tile}, /* first entry is default */
    {"><>", NULL}, /* no layout function means floating behavior */
    {"[M]", monocle},
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY, TAG)                                                      \
  {MODKEY, KEY, view, {.ui = 1 << TAG}},                                       \
      {MODKEY | ControlMask, KEY, toggleview, {.ui = 1 << TAG}},               \
      {MODKEY | ShiftMask, KEY, tag, {.ui = 1 << TAG}},                        \
      {MODKEY | ControlMask | ShiftMask, KEY, toggletag, {.ui = 1 << TAG}},

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd)                                                             \
  {                                                                            \
    .v = (const char *[]) { "/bin/sh", "-c", cmd, NULL }                       \
  }

/* commands */
static char dmenumon[2] =
    "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = {
    "dmenu_run", "-m",       dmenumon, "-fn",
    dmenufont,   "-nb",      col_bg, // normal background (dark)
    "-nf",       col_text,           // normal text (neon cyan)
    "-sb",       col_accent,         // selected background (hot pink)
    "-sf",       col_bar,            // selected text (dark to contrast)
    NULL};
static const char *termcmd[] = {"st", NULL};

static const char *browser[] = {"chromium-browser", NULL};
static const char *filemgr[] = {"thunar", NULL};
static const char *editor[] = {"nvim-gnome", NULL};
static const char *kittycmd[] = {"kitty", NULL};

static const Key keys[] = {
    /* modifier                     key        function        argument */
    {MODKEY, XK_p, spawn, {.v = dmenucmd}},
    {MODKEY | ShiftMask, XK_Return, spawn, {.v = termcmd}},
    {MODKEY, XK_b, togglebar, {0}},
    {MODKEY, XK_j, focusstack, {.i = +1}},
    {MODKEY, XK_k, focusstack, {.i = -1}},
    {MODKEY, XK_i, incnmaster, {.i = +1}},
    {MODKEY, XK_d, incnmaster, {.i = -1}},
    {MODKEY, XK_h, setmfact, {.f = -0.05}},
    {MODKEY, XK_l, setmfact, {.f = +0.05}},
    {MODKEY, XK_Return, zoom, {0}},
    {MODKEY, XK_Tab, view, {0}},
    {MODKEY | ShiftMask, XK_c, killclient, {0}},
    {MODKEY, XK_t, setlayout, {.v = &layouts[0]}},
    {MODKEY, XK_f, setlayout, {.v = &layouts[1]}},
    {MODKEY, XK_m, setlayout, {.v = &layouts[2]}},
    {MODKEY, XK_space, setlayout, {0}},
    {MODKEY | ShiftMask, XK_space, togglefloating, {0}},
    {MODKEY, XK_0, view, {.ui = ~0}},
    {MODKEY | ShiftMask, XK_0, tag, {.ui = ~0}},
    {MODKEY, XK_comma, focusmon, {.i = -1}},
    {MODKEY, XK_period, focusmon, {.i = +1}},
    {MODKEY | ShiftMask, XK_comma, tagmon, {.i = -1}},
    {MODKEY | ShiftMask, XK_period, tagmon, {.i = +1}},
    TAGKEYS(XK_1, 0) TAGKEYS(XK_2, 1) TAGKEYS(XK_3, 2) TAGKEYS(XK_4, 3)
        TAGKEYS(XK_5, 4) TAGKEYS(XK_6, 5) TAGKEYS(XK_7, 6) TAGKEYS(XK_8, 7)
            TAGKEYS(XK_9, 8){MODKEY | ShiftMask, XK_q, quit, {0}},

    {MODKEY, XK_w, spawn, {.v = browser}},
    {MODKEY, XK_f, spawn, {.v = filemgr}},
    {MODKEY | ShiftMask, XK_e, spawn, {.v = editor}},
    {MODKEY | ShiftMask, XK_Return, spawn, {.v = kittycmd}},
    {MODKEY | ShiftMask, XK_l, spawn, SHCMD("~/.local/bin/lock.sh")},

    /* Volume control */
    {0, XF86XK_AudioRaiseVolume, spawn,
     SHCMD("wpctl set-volume @DEFAULT_AUDIO_SINK@ 5%+")},
    {0, XF86XK_AudioLowerVolume, spawn,
     SHCMD("wpctl set-volume @DEFAULT_AUDIO_SINK@ 5%-")},
    {0, XF86XK_AudioMute, spawn,
     SHCMD("wpctl set-mute @DEFAULT_AUDIO_SINK@ toggle")},

    /* Brightness control */
    {0, XF86XK_MonBrightnessUp, spawn, SHCMD("brightnessctl set +5%")},
    {0, XF86XK_MonBrightnessDown, spawn, SHCMD("brightnessctl set 5%-")},

};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle,
 * ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
    /* click                event mask      button          function argument */
    {ClkLtSymbol, 0, Button1, setlayout, {0}},
    {ClkLtSymbol, 0, Button3, setlayout, {.v = &layouts[2]}},
    {ClkWinTitle, 0, Button2, zoom, {0}},
    {ClkStatusText, 0, Button2, spawn, {.v = termcmd}},
    {ClkClientWin, MODKEY, Button1, movemouse, {0}},
    {ClkClientWin, MODKEY, Button2, togglefloating, {0}},
    {ClkClientWin, MODKEY, Button3, resizemouse, {0}},
    {ClkTagBar, 0, Button1, view, {0}},
    {ClkTagBar, 0, Button3, toggleview, {0}},
    {ClkTagBar, MODKEY, Button1, tag, {0}},
    {ClkTagBar, MODKEY, Button3, toggletag, {0}},

};
