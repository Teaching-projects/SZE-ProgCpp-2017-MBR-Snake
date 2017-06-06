asm volatile (".code16gcc\n\t");

#include <iterator>
#include <array>
#include <utility>
#include <algorithm>
using namespace std;

template <typename T>
struct vec2 {
    vec2(T X, T Y) : x(X), y(Y) {}
    vec2() {}
    bool operator==(const vec2& other) {
        return (x == other.x) && (y == other.y);
    }
    T x{};
    T y{};
};

using i16Vec2 = vec2<int16_t>;

class x86 {
private:

    struct rgbStruct {
        uint8_t r{ 0 };
        uint8_t g{ 0 };
        uint8_t b{ 0 };
    };

    template<uint8_t red, uint8_t green, uint8_t blue>
    static constexpr bool colorComp(const rgbStruct& a, const rgbStruct& b) {
        return
        (((a.r - red) * (a.r - red) + (a.g - green) * (a.g - green) + (a.b - blue) * (a.b - blue))
        <
        ((b.r - red) * (b.r - red) + (b.g - green) * (b.g - green) + (b.b - blue) * (b.b - blue)
        ));
    }

public:

    static void* const memStart;

    static void setVideoMode(uint8_t mode) {
        asm volatile("xor %%ah, %%ah\n\tint $0x10" : : "al"(mode));
    }

    static void printChar(char c, uint8_t color = 7) {
        asm volatile("int $0x10" : : "a"(0x0e00 | c), "b"(color));
    }
    
    static void printString(const char* c, uint8_t color = 7) {
        while (*c) {
            printChar(*c, color);
            advance(c, 1);
        }
    }
    
    static auto readKeyboardChar() {
        uint16_t ax{ 0x0 };
        asm volatile("int $0x16" : "+a"(ax));
        return static_cast<char>(ax);
    }
    
    static auto readKeyboardScanCode() {
        uint16_t ax{ 0x0 };
        asm volatile("int $0x16" : "+a"(ax));
        return static_cast<char>(ax >> 8);
    }

    static bool isKeyAvailable() {
        uint16_t ax{ 0x0 };
        asm volatile("mov $1, %%ah\n\tint $0x16\n\tjz .ret_key_detected\n\tmov $1,%%al\n.ret_key_detected:" : "=a"(ax));
        return (ax & 0x1);
    }
    
    static void setPixel(uint8_t color, int16_t x, int16_t y) {
        asm volatile("mov $0x0c, %%ah\n\txor %%bh, %%bh\n\tint $0x10" : : "al"(color), "c"(x), "d"(y));
    }
    
    template <uint8_t red, uint8_t green, uint8_t blue>
    static constexpr uint8_t rgb() {
        const rgbStruct colorTable[] = {
            rgbStruct{ 0, 0, 0 },
            rgbStruct{ 0, 0, 85 },
            rgbStruct{ 0, 85, 0 },
            rgbStruct{ 0, 85, 85 },
            rgbStruct{ 85, 0, 0 },
            rgbStruct{ 85, 0, 85 },
            rgbStruct{ 85, 85, 0 },
            rgbStruct{ 170, 170, 170 },
            rgbStruct{ 85, 85, 85 },
            rgbStruct{ 0, 0, 170 },
            rgbStruct{ 0, 170, 0 },
            rgbStruct{ 0, 170, 170 },
            rgbStruct{ 170, 0, 0 },
            rgbStruct{ 170, 0, 170 },
            rgbStruct{ 170, 170, 85 },
            rgbStruct{ 255, 255, 255 }
        };
        return (min_element(begin(colorTable), end(colorTable), colorComp<red, green, blue>) - begin(colorTable));
    }

    static uint16_t getTicks() {
        uint16_t ticksL{ 0 }, ticksH{ 0 };
        asm volatile("int $0x1a" : "=c"(ticksH), "=d"(ticksL) : "a"(0x0));
        return ticksL;
    }

    static void clearScreen() {
        x86::setVideoMode(0x0d);
    }
    
    class frame {
        public:
        frame(uint16_t nFrames) {
            auto initialTicks = getTicks();
            while (getTicks() - initialTicks < nFrames) {}
        }
        ~frame() {}
    };
};

void* const x86::memStart{ (void*)0x7E00 };

class snek {
public:
    snek(const i16Vec2& startingPosition, int16_t startingDirection) : m_startLen{ 1 }, m_startPos{ startingPosition }, m_startDir{ startingDirection } {
        segment(0) = startingPosition;
    }

    void reset() {
        m_len = m_startLen;
        m_dir = m_startDir;
        segment(0) = m_startPos;
    }

    __attribute__((always_inline)) bool update(const i16Vec2& foodPos) {

        if (x86::isKeyAvailable()) {

            auto keyScan = x86::readKeyboardScanCode();
            
            if (keyScan == 0x4B) {
                // left
                m_dir = 3;
            }

            else if (keyScan == 0x4D) {
                // right
                m_dir = 1;
            }

            else if (keyScan == 0x48) {
                // up
                m_dir = 0;
            }

            else if (keyScan == 0x50) {
                // down
                m_dir = 2;
            }
        }


        auto i = m_len + 1;

        while (i --> 1) {
            segment(i) = segment(i - 1);
        }

        if (m_dir == 0) segment(0).y--;
        else if (m_dir == 1) segment(0).x++;
        else if (m_dir == 2) segment(0).y++;
        else if (m_dir == 3) segment(0).x--;

        if (segment(0) == foodPos) {
            // m_len++;
            return true;
        } else return false;
    }

    void draw() {
        x86::setPixel(m_headColor, segment(0).x, segment(0).y);
        for (int16_t i{ 1 }; i < m_len; i++) {
            x86::setPixel(m_bodyColor, segment(i).x, segment(i).y);
        }
    }

    int16_t getLen() const { return m_len; }

    int16_t getDir() const { return m_dir; }

    void setDirection(int16_t newDir) {
        m_dir = newDir;
    }

    void setLen(int16_t newLen) {
        m_len = newLen;
    }

    __attribute__((always_inline)) i16Vec2& segment(const int16_t& n) { return *(((i16Vec2*)x86::memStart) + n); }

private:
    int16_t m_len{ 1 };
    int16_t m_dir{ 1 };
    const int16_t m_startLen;
    const int16_t m_startDir;
    const i16Vec2 m_startPos;
    const uint8_t m_headColor{ x86::rgb<255, 255, 255>() };
    const uint8_t m_bodyColor{ x86::rgb<0, 255, 0>() };
};

class game {
public:
    game(const i16Vec2& gameAreaSize) : m_gameAreaSize{ gameAreaSize }, player({ 5, gameAreaSize.y / 2 }, 1), m_foodPos(gameAreaSize.x / 2, gameAreaSize.y / 2)  {}

    __attribute__((always_inline)) void update() {
        if (player.update(m_foodPos)) newFood();
        if (player.segment(0).x < 1 || player.segment(0).x > m_gameAreaSize.x + 1 || player.segment(0).y < 1 || player.segment(0).y > m_gameAreaSize.y + 1) player.reset();
    }

    void draw() {
        x86::clearScreen();
        drawBorder();
        player.draw();
        drawFood();
    }

private:
    snek player;
    i16Vec2 m_foodPos;
    const i16Vec2 m_gameAreaSize;
    const uint8_t m_foodColor{ x86::rgb<0, 255, 255>() };

    void drawBorder() {
        for (int16_t i{ 0 }; i < m_gameAreaSize.x + 3; i++) {
            x86::setPixel(x86::rgb<255, 255, 0>(), i, 0);
            x86::setPixel(x86::rgb<255, 255, 0>(), i, m_gameAreaSize.y + 2);
        }

        for (int16_t i{ 0 }; i < m_gameAreaSize.y + 2; i++) {
            x86::setPixel(x86::rgb<255, 255, 0>(), 0, i);
            x86::setPixel(x86::rgb<255, 255, 0>(), m_gameAreaSize.x + 2, i);
        }
    }

    void drawFood() {
        x86::setPixel(m_foodColor, m_foodPos.x, m_foodPos.y);
    }

    void newFood() {
        auto ticks = x86::getTicks();
        while (true) {
            m_foodPos = i16Vec2( (ticks % m_gameAreaSize.x) + 1, (ticks % m_gameAreaSize.y) + 1 );
            bool overlap = false;
            for (int16_t i{ 0 }; i < player.getLen(); i++) {
                if (player.segment(i) == m_foodPos) {
                    overlap = true;
                    break;
                }
            }
            if (overlap) {
                ticks << 0x1;
            } else break;
        }
    }
};

void main() {
    x86::setVideoMode(0x0d);

    game snakeGame({ 26, 14 });
    
    while (true) {

        x86::frame f(2);

        snakeGame.update();
        
        snakeGame.draw();
    }
}