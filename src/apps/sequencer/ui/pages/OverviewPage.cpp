#include "OverviewPage.h"

#include "model/NoteTrack.h"

#include "ui/painters/WindowPainter.h"

static void drawNoteTrack(Canvas &canvas, int trackIndex, const NoteTrackEngine &trackEngine, const NoteSequence &sequence) {
    canvas.setBlendMode(BlendMode::Set);

    int stepOffset = (std::max(0, trackEngine.currentStep()) / 16) * 16;
    int y = trackIndex * 8;

    for (int i = 0; i < 16; ++i) {
        int stepIndex = stepOffset + i;
        const auto &step = sequence.step(stepIndex);

        int x = 68 + i * 8;

        if (trackEngine.currentStep() == stepIndex) {
            canvas.setColor(step.gate() ? Color::Bright : Color::MediumBright);
            canvas.fillRect(x + 1, y + 1, 6, 6);
        } else {
            canvas.setColor(step.gate() ? Color::Medium : Color::Low);
            canvas.fillRect(x + 1, y + 1, 6, 6);
        }

        // if (trackEngine.currentStep() == stepIndex) {
        //     canvas.setColor(Color::Bright);
        //     canvas.drawRect(x + 1, y + 1, 6, 6);
        // }
    }
}

static void drawCurve(Canvas &canvas, int x, int y, int w, int h, float &lastY, const Curve::Function function, float min, float max) {
    const int Step = 1;

    auto eval = [=] (float x) {
        return (1.f - (function(x) * (max - min) + min)) * h;
    };

    float fy0 = y + eval(0.f);

    if (lastY >= 0.f && lastY != fy0) {
        canvas.line(x, lastY, x, fy0);
    }

    for (int i = 0; i < w; i += Step) {
        float fy1 = y + eval((float(i) + Step) / w);
        canvas.line(x + i, fy0, x + i + Step, fy1);
        fy0 = fy1;
    }

    lastY = fy0;
}

static void drawCurveTrack(Canvas &canvas, int trackIndex, const CurveTrackEngine &trackEngine, const CurveSequence &sequence) {
    canvas.setBlendMode(BlendMode::Add);
    canvas.setColor(Color::MediumBright);

    int stepOffset = (std::max(0, trackEngine.currentStep()) / 16) * 16;
    int y = trackIndex * 8;

    float lastY = -1.f;

    for (int i = 0; i < 16; ++i) {
        int stepIndex = stepOffset + i;
        const auto &step = sequence.step(stepIndex);
        float min = step.minNormalized();
        float max = step.maxNormalized();
        const auto function = Curve::function(Curve::Type(std::min(Curve::Last - 1, step.shape())));

        int x = 68 + i * 8;

        drawCurve(canvas, x, y + 1, 8, 6, lastY, function, min, max);
    }

    if (trackEngine.currentStep() >= 0) {
        int x = 64 + ((trackEngine.currentStep() - stepOffset) + trackEngine.currentStepFraction()) * 8;
        canvas.setBlendMode(BlendMode::Set);
        canvas.setColor(Color::Bright);
        canvas.vline(x, y + 1, 7);
    }
}


OverviewPage::OverviewPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{}

void OverviewPage::enter() {
}

void OverviewPage::exit() {
}

void OverviewPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);

    canvas.setFont(Font::Tiny);
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(Color::Medium);

    canvas.vline(68 - 3, 0, 68);
    canvas.vline(68 - 2, 0, 68);
    canvas.vline(196 + 1, 0, 68);
    canvas.vline(196 + 2, 0, 68);

    for (int trackIndex = 0; trackIndex < 8; trackIndex++) {
        const auto &track = _project.track(trackIndex);
        const auto &trackState = _project.playState().trackState(trackIndex);
        const auto &trackEngine = _engine.trackEngine(trackIndex);

        canvas.setBlendMode(BlendMode::Set);
        canvas.setColor(Color::Medium);

        int y = 5 + trackIndex * 8;

        // track number / pattern number
        canvas.setColor(trackState.mute() ? Color::Medium : Color::Bright);
        switch (track.trackMode()) {
            case Track::TrackMode::Note:
                canvas.drawText(2, y, track.noteTrack().name());
                break;
            case Track::TrackMode::Curve:
                canvas.drawText(2, y, track.curveTrack().name());
                break;
            case Track::TrackMode::MidiCv:
                canvas.drawText(2, y, track.midiCvTrack().name());
                break;
            default:
                break;
        }  

        std::string s = std::to_string(trackState.pattern() + 1);
        char const *pchar = s.c_str(); 
        char const p[] = {'P'};

        canvas.fillRect(46 - 1, y - 5, canvas.textWidth(p)+canvas.textWidth(pchar) + 1, 7);
        canvas.setBlendMode(BlendMode::Sub);
        canvas.drawText(46, y, FixedStringBuilder<8>("P%d", trackState.pattern() + 1));
        canvas.setBlendMode(BlendMode::Set);
        // gate output
        bool gate = _engine.gateOutput() & (1 << trackIndex);
        canvas.setColor(gate ? Color::Bright : Color::Medium);
        canvas.fillRect(256 - 48 + 1, trackIndex * 8 + 1, 6, 6);

        // cv output
        canvas.setColor(Color::Bright);
        canvas.drawText(256 - 32, y, FixedStringBuilder<8>("%.2fV", _engine.cvOutput().channel(trackIndex)));

        switch (track.trackMode()) {
        case Track::TrackMode::Note:
            drawNoteTrack(canvas, trackIndex, trackEngine.as<NoteTrackEngine>(), track.noteTrack().sequence(trackState.pattern()));
            break;
        case Track::TrackMode::Curve:
            drawCurveTrack(canvas, trackIndex, trackEngine.as<CurveTrackEngine>(), track.curveTrack().sequence(trackState.pattern()));
            break;
        case Track::TrackMode::MidiCv:
            break;
        case Track::TrackMode::Last:
            break;
        }
    }
}

void OverviewPage::updateLeds(Leds &leds) {
}

void OverviewPage::keyDown(KeyEvent &event) {
    const auto &key = event.key();

    if (key.isGlobal()) {
        return;
    }

    // event.consume();
}

void OverviewPage::keyUp(KeyEvent &event) {
    const auto &key = event.key();

    if (key.isGlobal()) {
        return;
    }

    // event.consume();
}

void OverviewPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.isGlobal()) {
        return;
    }

    // event.consume();
}

void OverviewPage::encoder(EncoderEvent &event) {
    // event.consume();
}
