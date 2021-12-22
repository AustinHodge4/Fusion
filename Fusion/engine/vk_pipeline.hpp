#pragma once

#include <engine/vk_types.hpp>
#include <iostream>
#include<vector>

namespace vk_pipeline {
	struct PipelineConfig {
		std::vector<vk::PipelineShaderStageCreateInfo> m_shaderStages;

		vk::PipelineVertexInputStateCreateInfo m_vertexInputInfo;
		vk::PipelineInputAssemblyStateCreateInfo m_inputAssembly;
		vk::Viewport m_viewport;
		vk::Rect2D m_scissor;
		std::vector<vk::DynamicState> m_dynamicStates;
		vk::PipelineViewportStateCreateInfo m_viewportState;
		vk::PipelineRasterizationStateCreateInfo m_rasterizer;
		vk::PipelineDepthStencilStateCreateInfo m_depthStencil;
		vk::PipelineColorBlendAttachmentState m_colorBlendAttachment;
		vk::PipelineMultisampleStateCreateInfo m_multisampling;
		vk::PipelineLayout m_pipelineLayout;

		vk::Pipeline build_pipeline(vk::Device p_device, const vk::RenderPass& p_renderPass) {
			bool usingDynamicStates = m_dynamicStates.size() > 0;

			vk::PipelineViewportStateCreateInfo viewportState{}; // Combine all the viewports and states since some GPUs can support multiple
			viewportState.viewportCount = 1;
			viewportState.scissorCount = 1;

			if (usingDynamicStates) {
				viewportState.pViewports = nullptr; // Using dynamic
				viewportState.pScissors = nullptr; // Using dynamic
			}
			else {
				viewportState.pViewports = &m_viewport;
				viewportState.pScissors = &m_scissor; 
			}

			vk::PipelineColorBlendStateCreateInfo colorBlending{};
			colorBlending.logicOpEnable = VK_FALSE; // set to true to use second way of blending instead of the options above. This will automatically set blend to false for every framebuffer
			colorBlending.logicOp = vk::LogicOp::eCopy;
			colorBlending.attachmentCount = 1;
			colorBlending.pAttachments = &m_colorBlendAttachment;

			vk::GraphicsPipelineCreateInfo pipelineInfo{}; // Build pipeline info
			pipelineInfo.stageCount = m_shaderStages.size();
			pipelineInfo.pStages = m_shaderStages.data();
			pipelineInfo.pVertexInputState = &m_vertexInputInfo;
			pipelineInfo.pInputAssemblyState = &m_inputAssembly;
			pipelineInfo.pViewportState = &viewportState;
			pipelineInfo.pRasterizationState = &m_rasterizer;
			pipelineInfo.pMultisampleState = &m_multisampling;
			pipelineInfo.pDepthStencilState = &m_depthStencil;
			pipelineInfo.pColorBlendState = &colorBlending;
			if (usingDynamicStates) {
				vk::PipelineDynamicStateCreateInfo dynamicState{}; // Use dynamic states to change viewport and line width without recreating the pipeling
				dynamicState.dynamicStateCount = m_dynamicStates.size();
				dynamicState.pDynamicStates = m_dynamicStates.data();

				pipelineInfo.pDynamicState = &dynamicState; // Optional
			}
			pipelineInfo.layout = m_pipelineLayout;
			pipelineInfo.renderPass = p_renderPass;
			pipelineInfo.subpass = 0;
			pipelineInfo.basePipelineHandle = nullptr; // a base pipeline to derive from

			try {
				return p_device.createGraphicsPipeline(nullptr, pipelineInfo).value;
			}
			catch (vk::SystemError err) {
				throw std::runtime_error("failed to create graphics pipeline!");
			}
		}
	};
}