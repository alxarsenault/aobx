#include <iostream>
#include "aobx/audiobox.hpp"
#include "aobx/multi_channel.hpp"
#include "aobx/one_pole_filter.hpp"
#include "aobx/obj_traits.hpp"

#include "aobx/engine.hpp"
#include "aobx/value.hpp"
#include "aobx/sine.hpp"
#include "aobx/buffer_player.hpp"

#include "aobx/cst_math.hpp"
#include "aobx/snd_file.hpp"
#include "aobx/snd_player.hpp"
#include "aobx/cache.hpp"

#include <axlib/axlib.hpp>
#include <fst/print.h>
#include <exception>

#include <limits>
#include <iomanip>
#include <complex>

class AudioEngine : public aobx::engine<float> {
public:
	AudioEngine()
		: _lfo_buffer(1, 1024)
	{
		_sine_wave.set_frequency(800);
		_lfo.set_frequency(5);
        
        _snd.open(RESOURCES_PATH + std::string("/Alesis-Fusion-Clean-Guitar-C3.wav"));
        _snd_player.set_snd_file(&_snd);
        _snd_player.set_play_type(aobx::snd_player<float>::play_type::repeat);
        _snd_player.set_speed(1.0);
        _snd_player.play();
        
        _buffer_player.set_buffer(&_snd.get_buffer());
        
        float freq = 44100.0f / (float)_snd.get_buffer()[0].size();
        _buffer_player.set_frequency(1.0 * freq);
	}

protected:
	virtual void callback(
		const aobx::multi_channel_buffer<float>& inputs, aobx::multi_channel_buffer<float>& output)
	{
		_lfo_buffer.fill(0.0);
		_lfo.process(aobx::mono_channel<float>(_lfo_buffer));
        
        // Lfo offset version 1.
        _lfo_buffer[0].operations([](float& v) { v = v * 0.5 + 0.5; });
//        _lfo_buffer[0].operations([](float& v) { v *= 0.5; },
//                                        [](float& v, std::size_t index) { v += 0.5; });
//        _lfo_buffer[0].chain_operations([](float& v) { v *= 0.5; },
//                                        [](float& v) { v += 0.5; });
        // Lfo offset version 2.
//        _lfo_buffer[0] *= 0.5;
//        _lfo_buffer[0] += 0.5;

//		_sine_wave.process(aobx::stereo_channel<float>(output));
        
        _buffer_player.process(aobx::stereo_channel<float>(output));
        
//        _snd_player.process(aobx::stereo_channel<float>(output));
        
//        for(std::size_t i = 0; i < output.chunk_size(); i++) {
//            _snd_player.process(aobx::stereo_value<float>(output[0][i], output[1][i]));
//        }
    

//		output[0] *= _lfo_buffer[0];
//		output[1] *= _lfo_buffer[0];
	}

private:
	aobx::sine<float> _sine_wave;
	aobx::sine<float> _lfo;
	aobx::multi_channel_buffer<float> _lfo_buffer;
    aobx::snd_file<float> _snd;
    aobx::snd_player<float> _snd_player;
    aobx::buffer_player<float> _buffer_player;
};

int main() {
    fst::print((int)aobx::cache::size());
    AudioEngine engine;
    std::vector<aobx::output_device> output_devices = engine.get_output_devices();
    
    for(auto& n : output_devices) {
        std::cout << n.name << std::endl;
    }
    
    engine.open(output_devices[0], 44100, 1024);
    engine.start();
    while(1) {
    }
    return 0;
}

// int main(int arc, char* argv[])
//{
//    // Get app instance.
//    ax::App& app(ax::App::GetInstance());
//
//    app.AddMainEntry([&app]() {
//        fst::print("App main entry.");
//        // Create main window.
//        ax::Window* win = ax::Window::Create(ax::Rect(0, 0, 500, 500));
//
//        // Set main window drawing callback.
//        win->event.OnPaint = ax::WFunc<ax::GC>([win](ax::GC gc) {
//            const ax::Rect rect(win->dimension.GetRect());
//            // Draw window background.
//            gc.SetColor(ax::Color(0.4));
//            gc.DrawRectangle(rect);
//        });
//
//        // Set main window.
//        app.AddTopLevel(std::shared_ptr<ax::Window>(win));
//    });
//
//    // Reset frame size after GUI is loaded.
//    app.AddAfterGUILoadFunction([&app]() { app.SetFrameSize(ax::Size(500, 500)); });
//
//    // Start main loop.
//    // Nothing after this line will be executed.
//    app.MainLoop();
//
//    return 0;
//}
