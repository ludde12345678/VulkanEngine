#pragma once



struct RenderSettings
{
	bool shadowMap;
};

struct DebugRenderSettings
{
	bool dispDebugWindow = false;
	bool wireframe = false;
};

struct Settings
{
	RenderSettings renderSettings;
	DebugRenderSettings debugRenderSettings;
};