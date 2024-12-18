#pragma once

#include "Sleet/Core/Layer.h"

namespace Sleet {

	class ImGuiLayer : public Layer
	{
	public:
		~ImGuiLayer() = default;

		static ImGuiLayer* Create();

		virtual void OnAttach() = 0;
		virtual void OnDetach() = 0;
		virtual void OnImGuiRender() = 0;

		virtual void Begin() = 0;
		virtual void End() = 0;
	private:

	};

}