#pragma once

enum class e_client_frame_stage {
	undefined = -1,
	start,
	net_update_start,
	net_update_postdataupdate_start,
	net_update_postdataupdate_end,
	net_update_end,
	render_start,
	render_end,
	net_full_frame_update_on_remove
};