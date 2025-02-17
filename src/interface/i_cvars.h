#pragma once
/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "interface/i_types.h"

//=============================================================================
// cvars
// cv_ prepended to avoid namespace pollution
//=============================================================================

extern cvar_t cv_chase_back;
extern cvar_t cv_chase_up;
extern cvar_t cv_chase_right;
extern cvar_t cv_chase_active;
extern cvar_t cv_cl_upspeed;
extern cvar_t cv_cl_forwardspeed;
extern cvar_t cv_cl_backspeed;
extern cvar_t cv_cl_sidespeed;
extern cvar_t cv_cl_movespeedkey;
extern cvar_t cv_cl_yawspeed;
extern cvar_t cv_cl_pitchspeed;
extern cvar_t cv_cl_anglespeedkey;
extern cvar_t cv_cl_name;
extern cvar_t cv_cl_color;
extern cvar_t cv_cl_shownet;
extern cvar_t cv_cl_nolerp;
extern cvar_t cv_lookspring;
extern cvar_t cv_lookstrafe;
extern cvar_t cv_sensitivity;
extern cvar_t cv_m_pitch;
extern cvar_t cv_m_yaw;
extern cvar_t cv_m_forward;
extern cvar_t cv_m_side;
extern cvar_t cv_registered;
extern cvar_t cv_cmdline;
extern cvar_t cv_con_notifytime;
extern cvar_t cv_d_subdiv16;
extern cvar_t cv_d_mipcap;
extern cvar_t cv_d_mipscale;
extern cvar_t cv_gl_nobind;
extern cvar_t cv_gl_max_size;
extern cvar_t cv_gl_picmip;
extern cvar_t cv_gl_subdivide_size;
extern cvar_t cv_r_norefresh;
extern cvar_t cv_r_speeds;
extern cvar_t cv_r_lightmap;
extern cvar_t cv_r_shadows;
extern cvar_t cv_r_mirroralpha;
extern cvar_t cv_r_wateralpha;
extern cvar_t cv_r_dynamic;
extern cvar_t cv_r_novis;
extern cvar_t cv_gl_finish;
extern cvar_t cv_gl_clear;
extern cvar_t cv_gl_cull;
extern cvar_t cv_gl_texsort;
extern cvar_t cv_gl_smoothmodels;
extern cvar_t cv_gl_affinemodels;
extern cvar_t cv_gl_polyblend;
extern cvar_t cv_gl_flashblend;
extern cvar_t cv_gl_playermip;
extern cvar_t cv_gl_nocolors;
extern cvar_t cv_gl_keeptjunctions;
extern cvar_t cv_gl_reporttjunctions;
extern cvar_t cv_gl_doubleeyes;
extern cvar_t cv_gl_triplebuffer;
extern cvar_t cv_vid_redrawfull;
extern cvar_t cv_vid_waitforrefresh;
extern cvar_t cv_mouse_button_commands[3];
extern cvar_t cv_gl_ztrick;
extern cvar_t cv_host_framerate;
extern cvar_t cv_host_speeds;
extern cvar_t cv_sys_ticrate;
extern cvar_t cv_serverprofile;
extern cvar_t cv_fraglimit;
extern cvar_t cv_timelimit;
extern cvar_t cv_teamplay;
extern cvar_t cv_samelevel;
extern cvar_t cv_noexit;
extern cvar_t cv_developer;
extern cvar_t cv_skill;
extern cvar_t cv_deathmatch;
extern cvar_t cv_coop;
extern cvar_t cv_pausable;
extern cvar_t cv_temp1;
extern cvar_t cv_m_filter;
extern cvar_t cv_in_joystick;
extern cvar_t cv_joy_name;
extern cvar_t cv_joy_advanced;
extern cvar_t cv_joy_advaxisx;
extern cvar_t cv_joy_advaxisy;
extern cvar_t cv_joy_advaxisz;
extern cvar_t cv_joy_advaxisr;
extern cvar_t cv_joy_advaxisu;
extern cvar_t cv_joy_advaxisv;
extern cvar_t cv_joy_forwardthreshold;
extern cvar_t cv_joy_sidethreshold;
extern cvar_t cv_joy_pitchthreshold;
extern cvar_t cv_joy_yawthreshold;
extern cvar_t cv_joy_forwardsensitivity;
extern cvar_t cv_joy_sidesensitivity;
extern cvar_t cv_joy_pitchsensitivity;
extern cvar_t cv_joy_yawsensitivity;
extern cvar_t cv_joy_wwhack1;
extern cvar_t cv_joy_wwhack2;
extern cvar_t cv_net_messagetimeout;
extern cvar_t cv_hostname;
extern cvar_t cv_config_com_port;
extern cvar_t cv_config_com_irq;
extern cvar_t cv_config_com_baud;
extern cvar_t cv_config_com_modem;
extern cvar_t cv_config_modem_dialtype;
extern cvar_t cv_config_modem_clear;
extern cvar_t cv_config_modem_init;
extern cvar_t cv_config_modem_hangup;
extern cvar_t cv_sv_aim;
extern cvar_t cv_nomonsters;
extern cvar_t cv_gamecfg;
extern cvar_t cv_scratch1;
extern cvar_t cv_scratch2;
extern cvar_t cv_scratch3;
extern cvar_t cv_scratch4;
extern cvar_t cv_savedgamecfg;
extern cvar_t cv_saved1;
extern cvar_t cv_saved2;
extern cvar_t cv_saved3;
extern cvar_t cv_saved4;
extern cvar_t cv_r_draworder;
extern cvar_t cv_r_timegraph;
extern cvar_t cv_r_graphheight;
extern cvar_t cv_r_clearcolor;
extern cvar_t cv_r_waterwarp;
extern cvar_t cv_r_fullbright;
extern cvar_t cv_r_drawentities;
extern cvar_t cv_r_drawviewmodel;
extern cvar_t cv_r_aliasstats;
extern cvar_t cv_r_dspeeds;
extern cvar_t cv_r_drawflat;
extern cvar_t cv_r_ambient;
extern cvar_t cv_r_reportsurfout;
extern cvar_t cv_r_maxsurfs;
extern cvar_t cv_r_numsurfs;
extern cvar_t cv_r_reportedgeout;
extern cvar_t cv_r_maxedges;
extern cvar_t cv_r_numedges;
extern cvar_t cv_r_aliastransbase;
extern cvar_t cv_r_aliastransadj;
extern cvar_t cv_scr_viewsize;
extern cvar_t cv_scr_fov;
extern cvar_t cv_scr_conspeed;
extern cvar_t cv_scr_centertime;
extern cvar_t cv_scr_showram;
extern cvar_t cv_scr_showturtle;
extern cvar_t cv_scr_showpause;
extern cvar_t cv_scr_printspeed;
extern cvar_t cv_bgmvolume;
extern cvar_t cv_volume;
extern cvar_t cv_nosound;
extern cvar_t cv_precache;
extern cvar_t cv_loadas8bit;
extern cvar_t cv_bgmbuffer;
extern cvar_t cv_ambient_level;
extern cvar_t cv_ambient_fade;
extern cvar_t cv_snd_noextraupdate;
extern cvar_t cv_snd_show;
extern cvar_t cv_snd_mixahead;
extern cvar_t cv_sv_friction;
extern cvar_t cv_sv_stopspeed;
extern cvar_t cv_sv_gravity;
extern cvar_t cv_sv_maxvelocity;
extern cvar_t cv_sv_nostep;
extern cvar_t cv_sv_edgefriction;
extern cvar_t cv_sv_idealpitchscale;
extern cvar_t cv_sv_maxspeed;
extern cvar_t cv_sv_accelerate;
extern cvar_t cv_vid_mode;
extern cvar_t cv_vid_default_mode;
extern cvar_t cv_vid_default_mode_win;
extern cvar_t cv_vid_wait;
extern cvar_t cv_vid_nopageflip;
extern cvar_t cv_vid_wait_override;
extern cvar_t cv_vid_config_x;
extern cvar_t cv_vid_config_y;
extern cvar_t cv_vid_stretch_by_2;
extern cvar_t cv_windowed_mouse;
extern cvar_t cv_vid_fullscreen_mode;
extern cvar_t cv_vid_windowed_mode;
extern cvar_t cv_block_switch;
extern cvar_t cv_vid_window_x;
extern cvar_t cv_vid_window_y;
extern cvar_t cv_lcd_x;
extern cvar_t cv_lcd_yaw;
extern cvar_t cv_scr_ofsx;
extern cvar_t cv_scr_ofsy;
extern cvar_t cv_scr_ofsz;
extern cvar_t cv_cl_rollspeed;
extern cvar_t cv_cl_rollangle;
extern cvar_t cv_cl_bob;
extern cvar_t cv_cl_bobcycle;
extern cvar_t cv_cl_bobup;
extern cvar_t cv_v_kicktime;
extern cvar_t cv_v_kickroll;
extern cvar_t cv_v_kickpitch;
extern cvar_t cv_v_iyaw_cycle;
extern cvar_t cv_v_iroll_cycle;
extern cvar_t cv_v_ipitch_cycle;
extern cvar_t cv_v_iyaw_level;
extern cvar_t cv_v_iroll_level;
extern cvar_t cv_v_ipitch_level;
extern cvar_t cv_v_idlescale;
extern cvar_t cv_crosshair;
extern cvar_t cv_cl_crossx;
extern cvar_t cv_cl_crossy;
extern cvar_t cv_gl_cshiftpercent;
extern cvar_t cv_v_centermove;
extern cvar_t cv_v_centerspeed;
extern cvar_t cv_v_gamma;

void Cvars_RegisterAll(void);
