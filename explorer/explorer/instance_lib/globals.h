#ifndef GLOBALS_H
#define GLOBALS_H
#define dword DWORD
#include <Windows.h>
namespace globals {
	HANDLE h;
	DWORD task_scheduler_ptr = 0x3C49EE8; // correct ptr (0x3743528) = task scheduler settings;
	DWORD scriptcontext;
	DWORD datamodel;
	
}

namespace jobs {
	DWORD start_job = 0x134;
	DWORD end_job = 0x138;
	DWORD datamodel = 0x28;
	DWORD job_name = 0x10;
	DWORD script_context = 0x10;
}
namespace instance {
	DWORD parent_offset = 0x30;
	DWORD name_offset = 0x24;
	DWORD child_list = 0x2C;
	DWORD class_descriptor = 0xC;
	DWORD class_name = 0x4;
}

namespace players {

}

namespace player {

}

namespace team {
	DWORD team_name = 0x1A8;
}

namespace textlabel {
	DWORD text_offset = 0x64C;
}
#endif // !GLOBALS_H
