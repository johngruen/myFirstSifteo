/*
 * Sifteo SDK Example.
 */

#include <sifteo.h>
#include "assets.gen.h"
using namespace Sifteo;

static Metadata M = Metadata()
    .title("John's Hacky Thing")
    .package("com.sifteo.sdk.hack", "1.1")
    .icon(Icon)
    .cubeRange(0, CUBE_ALLOCATION);

static VideoBuffer vid[CUBE_ALLOCATION];
static TiltShakeRecognizer motion[CUBE_ALLOCATION];


class SensorListener {
public:
    struct Counter {
        unsigned touch;
      //  unsigned neighborAdd;
      //  unsigned neighborRemove;
    } counters[CUBE_ALLOCATION];

    void install()
    {
      //  Events::neighborAdd.set(&SensorListener::onNeighborAdd, this);
       // Events::neighborRemove.set(&SensorListener::onNeighborRemove, this);
        Events::cubeAccelChange.set(&SensorListener::onAccelChange, this);
        Events::cubeTouch.set(&SensorListener::onTouch, this);
        //Events::cubeBatteryLevelChange.set(&SensorListener::onBatteryChange, this);
        Events::cubeConnect.set(&SensorListener::onConnect, this);

        // Handle already-connected cubes
        for (CubeID cube : CubeSet::connected())
            onConnect(cube);
    }

private:
    void onConnect(unsigned id)
    {
        CubeID cube(id);
   
        uint64_t hwid = cube.hwID();

        bzero(counters[id]);
        LOG("Cube %d connected\n", id);

        vid[id].initMode(BG0_ROM);
        vid[id].attach(id);
        motion[id].attach(id);

        // Draw the cube's identity
        String<128> str;
        str << "I am cube #" << cube << "\n";
        str << "hwid " << Hex(hwid >> 32) << "\n     " << Hex(hwid) << "\n\n";
        vid[cube].bg0rom.text(vec(1,2), str);
       
        // Draw initial state for all sensors
        onAccelChange(cube);
       // onBatteryChange(cube);
        onTouch(cube);
       
    }

   /* void onBatteryChange(unsigned id)
    {
        CubeID cube(id);
        String<32> str;
        str << "bat:   " << FixedFP(cube.batteryLevel(), 1, 3) << "\n";
        vid[cube].bg0rom.text(vec(1,13), str);
    }*/

    void onTouch(unsigned id)
    {
        unsigned fg = BG0ROMDrawable::SOLID_FG ^ BG0ROMDrawable::RED;
        //unsigned bg = BG0ROMDrawable::SOLID_FG ^ BG0ROMDrawable::OxFFFF;
        CubeID cube(id);
        vid[id].initMode(BG0_ROM);
        vid[id].attach(cube);
        //vid[id].bg0rom.erase(bg);
        vid[id].bg0rom.fill(vec(0,0), vec(16,16), fg);

       
    }

    void onAccelChange(unsigned id)
    {
        unsigned fg;
        if (id % 3 == 0) {
            fg = BG0ROMDrawable::SOLID_FG ^ BG0ROMDrawable::BLUE;
        } else if (id % 3 == 1) {
             fg = BG0ROMDrawable::SOLID_FG ^ BG0ROMDrawable::GREEN;
        } else {
            fg = BG0ROMDrawable::SOLID_FG ^ BG0ROMDrawable::RED;
        }
        
        unsigned bg = BG0ROMDrawable::SOLID_FG ^ BG0ROMDrawable::BLACK;
        CubeID cube(id);
        vid[id].initMode(BG0_ROM);
        vid[id].attach(cube);
        vid[id].bg0rom.erase(bg);
        auto accel = cube.accel();
        auto tilt = motion[id].tilt;
        int x = (int)accel.x;
        int y = (int)accel.y;
        if (x <= 2) x = 2;
        if (x >= 14) x = 14;
        if (y <= 2) y = 2;
        if (y >= 14) y = 14;

        vid[id].bg0rom.fill(vec(0,0), vec(x,y), fg);
        
    
        
    }
    
    
  
};

void main()
{
    static SensorListener sensors;

    sensors.install();

    // We're entirely event-driven. Everything is
    // updated by SensorListener's event callbacks.
    while (1)
        System::paint();
}
