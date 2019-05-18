#pragma once

#include "TrackEngine.h"
#include "ArpeggiatorEngine.h"

#include "model/Track.h"

class MidiCvTrackEngine : public TrackEngine {
public:
    MidiCvTrackEngine(Engine &engine, const Model &model, Track &track, const TrackEngine *linkedTrackEngine) :
        TrackEngine(engine, model, track, linkedTrackEngine),
        _midiCvTrack(track.midiCvTrack()),
        _arpeggiatorEngine(_midiCvTrack.arpeggiator())
    {
        reset();
    }

    virtual Track::TrackMode trackMode() const override { return Track::TrackMode::MidiCv; }

    virtual void reset() override;
    virtual void tick(uint32_t tick) override;
    virtual void update(float dt) override;

    virtual bool receiveMidi(MidiPort port, const MidiMessage &message) override;

    virtual bool activity() const override;
    virtual bool gateOutput(int index) const override;
    virtual float cvOutput(int index) const override;

private:
    void updateArpeggiator();
    void tickArpeggiator(uint32_t tick);

    float noteToCv(int note) const;
    float valueToCv(int value) const;
    float pitchBendToCv(int value) const;

    struct Voice;

    void resetVoices();
    void addVoice(int note, int velocity);
    void removeVoice(int note);

    Voice *allocateVoice(int note, int numVoices);
    void freeVoice(int note, int numVoices);

    Voice *findVoice(int begin, int end, int note);
    Voice *lruVoice(int begin, int end);
    Voice *mruVoice(int begin, int end);

    void printVoices();

    void updateActivity();

    static constexpr int RetriggerDelay = 2;

    const MidiCvTrack &_midiCvTrack;

    ArpeggiatorEngine _arpeggiatorEngine;
    bool _arpeggiatorEnabled;
    float _arpeggiatorTime;
    uint32_t _arpeggiatorTick;

    struct Voice {
        uint32_t ticks = 0;
        uint8_t note;
        float pitchCv;
        float velocityCv;
        float pressureCv;
    };

    std::array<Voice, 8> _voices;

    bool _activity;

    float _pitchBendCv;
    float _channelPressureCv;
};
