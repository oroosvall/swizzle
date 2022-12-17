#ifndef STATS_HPP
#define STATS_HPP

#include <string>

#include <swizzle/gfx/Context.hpp>
#include <utils/FpsCounter.hpp>

std::string GetStatisticsText(common::Resource<swizzle::gfx::GfxContext> context,
                              common::Resource<swizzle::gfx::CommandBuffer> cmdBuff,
                              common::Resource<swizzle::gfx::Swapchain> swapchain, utils::FpsCounter& fpsCounter);

#endif