#include <gtest/gtest.h>

#include "TeCoreApplication.h"

namespace test {

class AppRenderer : public ::testing::Test
{
public:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }

private:
};

class Application : public te::CoreApplication
{
public:
    explicit Application(te::START_UP_DESC desc) : CoreApplication(desc) {}

    unsigned int FramesRendered = 0;

protected:
    void PostRender() override
    {
        FramesRendered++;
        StopMainLoop();
    }
};

static te::START_UP_DESC GetDefaultStartUpDesc(bool headless = true)
{
    return {
        .RenderAPI = TE_RENDER_API_MODULE,
        .Renderer = TE_RENDERER_MODULE,
        .Physics = TE_PHYSICS_MODULE,
        .Audio = TE_AUDIO_MODULE,
        .Gui = TE_GUI_MODULE,
        .WindowDesc = {
            .Mode = te::VideoMode(1024, 576),
            .MultisampleCount = 1,
            .Title = "Window",
            .HeadLess = headless
        },
        .Importers = {
            "TeFreeImgImporter", "TeObjectImporter", "TeShaderImporter",
            "TeFontImporter", "TeProjectImporter", "TeResourceImporter"
        },
        .Exporters = {
            "TeProjectExporter", "TeResourceExporter", "TeFreeImgExporter"
        },
        .EnabledRenderDoc = true
    };
}

TEST(AppRenderer, StartsAndStopsHeadlessApplication)
{
#if TE_PLATFORM != TE_PLATFORM_WIN32
    GTEST_SKIP() << "Headless rendering is only implemented for Direct3D 11.";
#else
    te::START_UP_DESC desc = GetDefaultStartUpDesc();
    if (desc.RenderAPI != TE_RENDER_API_MODULE_D3D11)
        GTEST_SKIP() << "Headless rendering is only implemented for Direct3D 11.";

    te::CoreApplication::StartUp<Application>(desc);
    auto& app = static_cast<Application&>(te::gCoreApplication());
    EXPECT_TRUE(te::CoreApplication::IsStarted());
    EXPECT_NE(app.GetWindow(), nullptr);
    if (app.GetWindow())
        EXPECT_TRUE(app.GetWindow()->GetDesc().HeadLess);

    app.RunMainLoop();
    EXPECT_EQ(app.FramesRendered, 1u);

    te::CoreApplication::ShutDown();
    EXPECT_FALSE(te::CoreApplication::IsStarted());
#endif
}

TEST(AppRenderer, StartsAndStopsWindowedApplication)
{
#if TE_PLATFORM != TE_PLATFORM_WIN32
    GTEST_SKIP() << "Windowed rendering is only implemented for Direct3D 11.";
#else
    te::START_UP_DESC desc = GetDefaultStartUpDesc(false);
    if (desc.RenderAPI != TE_RENDER_API_MODULE_D3D11)
        GTEST_SKIP() << "Windowed rendering is only implemented for Direct3D 11.";

    te::CoreApplication::StartUp<Application>(desc);
    auto& app = static_cast<Application&>(te::gCoreApplication());
    EXPECT_TRUE(te::CoreApplication::IsStarted());
    EXPECT_NE(app.GetWindow(), nullptr);
    if (app.GetWindow())
        EXPECT_FALSE(app.GetWindow()->GetDesc().HeadLess);

    app.RunMainLoop();
    EXPECT_EQ(app.FramesRendered, 1u);

    te::CoreApplication::ShutDown();
    EXPECT_FALSE(te::CoreApplication::IsStarted());
#endif
}

} // namespace test
