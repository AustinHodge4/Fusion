#pragma once

namespace Fusion {

	class GraphicsContext
	{
	public:
		virtual void Initialize() = 0;
		virtual void SwapBuffers() = 0;

		static GraphicsContext* Create(void* p_window);
	};
}