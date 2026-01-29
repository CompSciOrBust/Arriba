#include <arribaPrimitives.h>

namespace Arriba::Elements {
    class Button : public Arriba::Primitives::Quad {
        private:
            Arriba::Primitives::Text* text;
            std::vector<void (*)()> callbacks;

        public:
            Button();

            virtual void onFrame();
            void setText(const char* _text);
            void setText(const char32_t* _text);
            void registerCallback(void (*func)());
    };
}  //namespace Arriba::Elements
