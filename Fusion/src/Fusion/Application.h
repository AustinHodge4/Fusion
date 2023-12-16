#pragma once

#include "core.h"

namespace fusion {

	class FUSION_API application
	{
	public:
		application();
		virtual ~application();

		void run();
	};

	application* create_application();

}

