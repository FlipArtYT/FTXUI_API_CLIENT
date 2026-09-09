#include <ftxui/dom/elements.hpp>
#include <ftxui/component/app.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/component.hpp>
#include <thread>
#include "../include/simplerapiclient.hpp"

int main() {
    using namespace ftxui;

    std::string user_url;
    std::string response_output = "API Response here...";

    APIClient client;

    auto screen = ScreenInteractive::Fullscreen();

    auto response_display = vbox({
        paragraph(response_output) | flex | border,
    }) | flex;

    auto fetch_data = [&]() {
        response_output = "Fetching: " + user_url;

        std::thread([&, url = user_url]() {
            APIResponse response = client.get(url);

            response_output = response.body;
            int status_code = response.status;

            if (status_code != 0) {
                Color status_color = (status_code >= 200 && status_code < 300) 
                ? ftxui::Color::Green 
                : ftxui::Color::Red;

                response_display = vbox({
                    text("Status code: "),
                    text(std::to_string(status_code)) | color(status_color) | bold,
                    paragraph(response_output) | flex | yframe,
                }) | flex | border;

            } else {
                response_display = vbox({
                    paragraph(response_output) | flex | yframe | bold | color(Color::Red),
                }) | flex | border;
            }
            

            screen.Post(Event::Custom);
        }).detach();
    };

    InputOption option;
    option.multiline = false;
    option.on_enter = fetch_data;

    Component user_url_lineedit = Input(&user_url, "Enter URL...", option);

    auto layout = Container::Vertical({
        user_url_lineedit,
    });

    auto renderer = Renderer(layout, [&] {
        return vbox({
            vbox({
                text("=== API Client ===") | center | bold,
                response_display,
                hbox(text(" URL : "), user_url_lineedit->Render())
            }) | border | flex,

            paragraph("^Q: Quit") | inverted,
        });
    });

    renderer |= CatchEvent([&](Event event) {
        if (event == Event::CtrlQ) {
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });

    screen.Loop(renderer);
}