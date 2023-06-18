#pragma once

struct player_info_t {
	long long __pad0;

	union {
		long long m_xuid;

		struct {
			int m_xuidlow;
			int m_xuidhigh;
		};
	};

	char m_name[ 128 ];
	int m_userid;
	char m_guid[ 33 ];
	unsigned int m_friendsid;
	char m_friendsname[ 128 ];
	bool m_fakeplayer;
	bool m_ishltv;
	unsigned int customfiles[ 4 ];
	unsigned char filesdownloaded;
};