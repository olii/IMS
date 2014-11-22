

#include "simulator.h"

namespace sim {
void Run()
{
	Calendar::instance()->dumpCalendar();
	Calendar::instance()->destroy_instance();
}


}