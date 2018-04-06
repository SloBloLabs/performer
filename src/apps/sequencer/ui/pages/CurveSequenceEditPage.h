#pragma once

#include "BasePage.h"
#include "ContextMenu.h"

#include "ui/StepSelection.h"

class CurveSequenceEditPage : public BasePage {
public:
    CurveSequenceEditPage(PageManager &manager, PageContext &context);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual void keyDown(KeyEvent &event) override;
    virtual void keyUp(KeyEvent &event) override;
    virtual void keyPress(KeyPressEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

private:
    static const int StepCount = 16;

    enum class Mode : uint8_t {
        Shape,
        Min,
        Max,
    };

    static const char *modeName(Mode mode) {
        switch (mode) {
        case Mode::Shape:           return "SHAPE"; break;
        case Mode::Min:             return "MIN"; break;
        case Mode::Max:             return "MAX"; break;
        }
        return nullptr;
    }

    int stepOffset() const { return _page * StepCount; }

    void contextAction(int index);
    bool contextActionEnabled(int index) const;
    void initSequence();
    void copySequence();
    void pasteSequence();
    void duplicateSequence();

    ContextMenu _contextMenu;

    Mode _mode = Mode::Shape;
    int _page = 0;

    StepSelection<CONFIG_STEP_COUNT> _stepSelection;
};