#include <entt/entt.hpp>
#define SDL_VIDEO_DRIVER_X11
#include <SDL.h>
#include <SDL_syswm.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/bx.h>
#include <imgui.h>
// render imgui in sdl
#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_opengl3.h>

struct CounterContainer {
  int counter;
};
entt::registry registry;
entt::entity entity;

void bgfxInit()
{
  bgfx::init();
  bgfx::reset(800, 600, BGFX_RESET_VSYNC);

  // Enable debug text.
  bgfx::setDebug(BGFX_DEBUG_TEXT /*| BGFX_DEBUG_STATS*/);

  // Set view 0 clear state.
  bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0xf39c12ff, 1.0f, 0);
}

void imguiInit()
{
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
}

void enttInit()
{
  entity = registry.create();
  registry.emplace<CounterContainer>(entity, 0);
}

void update()
{
  registry.view<CounterContainer>().each([](const auto entity, CounterContainer& container) {
    container.counter++;
  });
}

void render()
{
  bgfx::setViewRect(0, 0, 0, uint16_t(800), uint16_t(600));
  bgfx::touch(0);
  bgfx::dbgTextClear();

  bgfx::dbgTextPrintf(0, 1, 0x4f, "Text from BGFX. Counter from entt: %d", registry.get<CounterContainer>(entity).counter);

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();
  ImGui::Begin("UI window");
  ImGui::Button("Button from IMGUI");
  ImGui::End();
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  bgfx::frame();
}

inline bool sdlSetWindow(SDL_Window *window)
{
  SDL_GLContext context = SDL_GL_CreateContext(window);
  SDL_SysWMinfo wmi;
  SDL_VERSION(&wmi.version);
  if (!SDL_GetWindowWMInfo(window, &wmi))
  {
    return false;
  }

  ImGui_ImplSDL2_InitForOpenGL(window, context);
  ImGui_ImplOpenGL3_Init();

  bgfx::PlatformData pd;
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
  pd.ndt = wmi.info.x11.display;
  pd.nwh = (void *)(uintptr_t)wmi.info.x11.window;
#elif BX_PLATFORM_OSX
  pd.ndt = NULL;
  pd.nwh = wmi.info.cocoa.window;
#elif BX_PLATFORM_WINDOWS
  pd.ndt = NULL;
  pd.nwh = wmi.info.win.window;
#elif BX_PLATFORM_STEAMLINK
  pd.ndt = wmi.info.vivante.display;
  pd.nwh = wmi.info.vivante.window;
#endif // BX_PLATFORM_
  pd.context = NULL;
  pd.backBuffer = NULL;
  pd.backBufferDS = NULL;
  bgfx::setPlatformData(pd);

  return true;
}

int main(void)
{
  SDL_Init(0);

  SDL_Window *window = SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

  imguiInit();
  sdlSetWindow(window);
  bgfx::renderFrame();

  bgfxInit();
  bgfx::frame();

  bool exit = false;
  SDL_Event event;
  enttInit();

  while (!exit)
  {
    update();
    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
      case SDL_QUIT:
        exit = true;
        break;

      case SDL_WINDOWEVENT:
      {
        const SDL_WindowEvent &wev = event.window;
        switch (wev.event)
        {
        case SDL_WINDOWEVENT_RESIZED:
        case SDL_WINDOWEVENT_SIZE_CHANGED:
          break;

        case SDL_WINDOWEVENT_CLOSE:
          exit = true;
          break;
        }
      }
      break;
      }
      ImGui_ImplSDL2_ProcessEvent(&event);
    }
    render();
  }

  bgfx::shutdown();

  SDL_DestroyWindow(window);
  SDL_Quit();
}