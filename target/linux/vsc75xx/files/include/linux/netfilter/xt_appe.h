#ifndef __IPT_APPE_H
#define __IPT_APPE_H
#define APPE_VERSION "0.10"

// get ap index
#define GET_AP_TYPE(apid)			(((apid) & 0xFF00) >> 8)
#define GET_AP_INDEX(apid)			( (apid) & 0x00FF)
#define GET_AP_SUBGROUP(apid)		(((apid) & 0x00F0) >> 4)
#define GET_AP_SUBGROUP_INDEX(apid)	( (apid) & 0x000F)
#define GET_AP_ENABLE(apid)			(1 << ((apid) & 0x000F))

// appe application type
#define APPE_TYPE_IM_ADV	0xF0
#define APPE_TYPE_IM		0xF1
#define APPE_TYPE_VOIP		0xF2
#define APPE_TYPE_P2P		0xF3
#define APPE_TYPE_PROT		0xF4
#define APPE_TYPE_TUNL		0xF5
#define APPE_TYPE_STRM		0xF6
#define APPE_TYPE_GAME		0xF7
#define APPE_TYPE_RCTL		0xF8
#define APPE_TYPE_WHD		0xF9

// application count
#define IM_ADV_NUM		24
#define IM_AP_NUM		21
#define VOIP_AP_NUM		6
#define P2P_AP_NUM		18
#define PROT_AP_NUM		20
#define TUNL_AP_NUM		17
#define STRM_AP_NUM		19
#define GAME_AP_NUM		4
#define RCTL_AP_NUM		14
#define WHD_AP_NUM		11

// command id, must different from application id
#define CMD_RULE_INDEX		0x1000 // rule index
#define CMD_SYSLOG_ENABLE	0x1001 // enable syslog
#define LOG_PASS            0x2000 // log action
#define LOG_BLOCK           0x2001 // log action
// application id
// im advanced
#define IM_MSN				0xF000 // MSN login
#define IM_MSN_MSG			0xF001 // MSN message
#define IM_MSN_FILE			0xF002 // MSN file transfer
#define IM_MSN_GAME			0xF003 // MSN game
#define IM_MSN_CONF			0xF004 // MSN conference
#define IM_MSN_OTHER		0xF005 // MSN other actions
#define IM_YM				0xF006 // Yahoo Messenger login
#define IM_YM_MSG			0xF007 // Yahoo Messenger message
#define IM_YM_FILE			0xF008 // Yahoo Messenger file transfer
#define IM_YM_GAME			0xF009 // Yahoo Messenger game
#define IM_YM_CONF			0xF00a // Yahoo Messenger conference
#define IM_YM_OTHER			0xF00b // Yahoo Messenger other actions
#define IM_AIM				0xF00c // AIM
#define IM_AIM_MSG			0xF00d // AIM message
#define IM_AIM_FILE			0xF00e // AIM file transfer
#define IM_AIM_GAME			0xF00f // AIM game
#define IM_AIM_CONF			0xF010 // AIM conference
#define IM_AIM_OTHER		0xF011 // AIM other actions
#define IM_ICQ				0xF012 // ICQ
#define IM_ICQ_MSG			0xF013 // ICQ message
#define IM_ICQ_FILE			0xF014 // ICQ file transfer
#define IM_ICQ_GAME			0xF015 // ICQ game
#define IM_ICQ_CONF			0xF016 // ICQ conference
#define IM_ICQ_OTHER		0xF017 // ICQ other actions
// im applications
#define IM_WEB				0xF100 // Web IM
#define IM_AIM67			0xF101 // AIM6/7
#define IM_QQTM				0xF102 // QQ/TM
#define IM_ICHAT			0xF103 // iChat
#define IM_JABBER			0xF104 // Jabber
#define IM_GOOGLECHAT		0xF105 // Google Chat
#define IM_XFIRE			0xF106 // XFire
#define IM_GADUGADU			0xF107 // GaduGadu
#define IM_PALTALK			0xF108 // Paltalk
#define IM_QNEXT			0xF109 // Qnext
#define IM_POCO				0xF10a // POCO
#define IM_ARES				0xF10b // Ares talk
#define IM_ALIWW			0xF10c // AliWW
#define IM_KC				0xF10d // KC
#define IM_LAVALAVA			0xF10e // Lava-Lava
#define IM_ICU2				0xF10f // ICU2
#define IM_ISPQ				0xF110 // iSpQ
#define IM_UC				0xF111 // UC
#define IM_MOBILEMSN		0xF112 // Mobile MSN
#define IM_BAIDUHI			0xF113 // BaiduHi
#define IM_FETION			0xF114 // Fetion

// voip applications
#define VOIP_SKYPE			0xF200 // Skype
#define VOIP_KUBAO			0xF201 // Kubao
#define VOIP_GIZMO			0xF202 // Gizmo
#define VOIP_SIPRTP			0xF203 // SIP/RTP protocol
#define VOIP_TELTEL			0xF204 // TelTel
#define VOIP_TEAMSPEAK		0xF205 // TeamSpeak

// p2p applications
#define P2P_SOULSEEK		0xF300 // SoulSeek protocol
#define P2P_EDONKEY			0xF301 // eDonkey protocol
#define P2P_FASTTRACK		0xF302 // FastTrack protocol
#define P2P_OPENFT			0xF303 // OpenFT protocol
#define P2P_GNUTELLA		0xF304 // Gnutella protocol
#define P2P_OPENNAP			0xF305 // OpenNap protocol
#define P2P_BITTORRENT		0xF306 // BitTorrent protocol
#define P2P_XUNLEI			0xF307 // Xunlei
#define P2P_VAGAA			0xF308 // Vagaa
#define P2P_PP365			0xF309 // PP365
#define P2P_POCO			0xF30a // POCO
#define P2P_CLUBBOX			0xF30b // Clubbox
#define P2P_ARES			0xF30c // Ares
#define P2P_EZPEER			0xF30d // ezPeer
#define P2P_PANDO			0xF30e // Pando
#define P2P_HUNTMINE		0xF30f // Huntmine
#define P2P_KUWO			0xF310 // Kuwo

// protocol
#define PROT_DNS			0xF400 // DNS
#define PROT_FTP			0xF401 // FTP
#define PROT_HTTP			0xF402 // HTTP
#define PROT_IMAP			0xF403 // IMAP
#define PROT_IRC			0xF404 // IRC
#define PROT_NNTP			0xF405 // NNTP
#define PROT_POP3			0xF406 // POP3
#define PROT_SMB			0xF407 // SMB
#define PROT_SMTP			0xF408 // SMTP
#define PROT_SNMP			0xF409 // SNMP
#define PROT_SSH			0xF40a // SSH
#define PROT_SSLTLS			0xF40b // SSL/TLS
#define PROT_TELNET			0xF40c // TELNET
#define PROT_MSSQL			0xF40d // MSSQL
#define PROT_MYSQL			0xF40e // MySQL
#define PROT_ORACLE			0xF40f // Oracle
#define PROT_POSTGRESQL		0xF410 // PostgreSQL
#define PROT_SYBASE			0xF411 // Sybase
#define PROT_DB2			0xF412 // DB2
#define PROT_INFORMIX		0xF413 // Informix

// misc
// tunnel applications
#define TUNL_SOCKS    		0xF500 // Socks4/5
#define TUNL_PGPNET			0xF501 // PGPNet
#define TUNL_HTTPPROXY		0xF502 // HTTP Proxy
#define TUNL_TOR			0xF503 // TOR
#define TUNL_VNN			0xF504 // VNN
#define TUNL_SOFTETHER		0xF505 // SoftEther
#define TUNL_MSTEREDO		0xF506 // MS Teredo
#define TUNL_ULTRASURF		0xF507 // UltraSurf
#define TUNL_HAMACHI		0xF508 // Hamachi
#define TUNL_HTTPTUNNEL		0xF509 // Ping Tunnel
#define TUNL_PINGTUNNEL		0xF50a // Ping Tunnel
#define TUNL_TINYVPN		0xF50b // TinyVPN
#define TUNL_REALTUNNEL		0xF50c // RealTunnel
#define TUNL_DYNAPASS		0xF50d // DynaPass
#define TUNL_ULTRAVPN		0xF50e // UltraVPN
#define TUNL_FREEU			0xF50f // FreeU
#define TUNL_SKYFIRE		0xF510 // Skyfire
// streaming applications (grouping in MISC)
#define STRM_MMS			0xF600 // MMS
#define STRM_RTSP			0xF601 // RTSP
#define STRM_TVANTS			0xF602 // TVAnts
#define STRM_PPSTREAM		0xF603 // PPStream
#define STRM_PPLIVE			0xF604 // PPLive
#define STRM_UUSEE			0xF605 // UUSee
#define STRM_NSPLAYER		0xF606 // NSPlayer
#define STRM_PCAST			0xF607 // PCast
#define STRM_TVKOO			0xF608 // TVKoo
#define STRM_SOPCAST		0xF609 // SopCast
#define STRM_UDLIVEX		0xF60a // UDLiveX
#define STRM_TVUPLAYER		0xF60b // TVUPlayer
#define STRM_MYSEE			0xF60c // MySee
#define STRM_JOOST			0xF60d // Joost
#define STRM_FLASHVIDEO		0xF60e // FlashVideo
#define STRM_SILVERLIGHT	0xF60f // SilverLight
#define STRM_SLINGBOX		0xF610 // Slingbox
#define STRM_QVOD			0xF611 // QVOD
#define STRM_QQLIVE			0xF612 // QQLive
// small online game applications (grouping in MISC)
#define GAME_CNGAMES		0xF700 // China Games
#define GAME_SHGAMES		0xF701 // ShangHai Games
#define GAME_LZGAMES		0xF702 // LianZhong Games
#define GAME_BFGAMES		0xF703 // BianFeng Games
// remote control applications (grouping in MISC)
#define RCTL_VNC			0xF800 // VNC
#define RCTL_RADMIN			0xF801 // Radmin
#define RCTL_SPYANYWHERE	0xF802 // SpyAnywhere
#define RCTL_SHOWMYPC		0xF803 // ShowMyPC
#define RCTL_LOGMEIN		0xF804 // LogMeIn
#define RCTL_TEAMVIEWER		0xF805 // Team Viewer
#define RCTL_GOGROK			0xF806 // Gogrok
#define RCTL_RCONTROLPRO	0xF807 // Remote Control Pro
#define RCTL_CROSSLOOP		0xF808 // CrossLoop
#define RCTL_WINDOWSRDP		0xF809 // Windows Remote Desktop
#define RCTL_PCANYWHERE		0xF80a // pcAnywhere
#define RCTL_TIMBUKTU		0xF80b // Timbuktu
#define RCTL_WINLIVESYNC	0xF80c // WindowsLiveSync
#define RCTL_SHAREDVIEW		0xF80d // SharedView
// webhd applications (grouping in MISC)
#define WHD_HTTPUPLOAD		0xF900 // HTTP Upload
#define WHD_HINETSAFEBOX	0xF901 // Hinet SafeBox
#define WHD_MSSKYDRIVE		0xF902 // MS SkyDrive
#define WHD_GDOCUPLOADER	0xF903 // GDoc Uploader
#define WHD_ADRIVE			0xF904 // ADrive
#define WHD_MYOTHERDRIVE	0xF905 // MyOtherDrive
#define WHD_MOZY			0xF906 // MyOtherDrive
#define WHD_BOXNET			0xF907 // BoxNet
#define WHD_OFFICELIVE		0xF908 // OfficeLive
#define WHD_READDLESTORAGE	0xF909 // Readdle Storage
#define WHD_DROPBOX			0xF90a // Dropbox

/* You have to check whether the below condition is true after modify the structure definition
 *      XT_ALIGN(sizeof(struct ipt_appe_info)) == sizeof(struct ipt_appe_info)
 */
struct ipt_appe_info {
    int rule_idx;
    short syslog_enable;
    short block;
    int im_adv_cfg[2];
    int im_cfg[2];
    int voip_cfg[2];
    int p2p_cfg[2];
    int prot_cfg[2];
    int tunl_cfg[2];
    int strm_cfg[2];
    int game_cfg[2];
    int rctl_cfg[2];
    int whd_cfg[2];
};

#endif //__IPT_APPE_H
