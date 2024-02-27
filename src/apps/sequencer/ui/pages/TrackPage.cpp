#include "TrackPage.h"

#include "Pages.h"

#include "ui/LedPainter.h"
#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

enum class ContextAction {
    Init,
    Copy,
    Paste,
    Route,
    Last
};

static const ContextMenuModel::Item contextMenuItems[] = {
    { "INIT" },
    { "COPY" },
    { "PASTE" },
    { "ROUTE" },
};

TrackPage::TrackPage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _noteTrackListModel)
{}

void TrackPage::enter() {
    setTrack(_project.selectedTrack());
}

void TrackPage::exit() {
}

void TrackPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "TRACK");
    WindowPainter::drawActiveFunction(canvas, Track::trackModeName(_project.selectedTrack().trackMode()));
    WindowPainter::drawFooter(canvas);

    ListPage::draw(canvas);
}

void TrackPage::updateLeds(Leds &leds) {
    ListPage::updateLeds(leds);
}

void TrackPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.isContextMenu()) {
        contextShow();
        event.consume();
        return;
    }


    if (key.pageModifier() && event.count() == 2) {
        contextShow(true);
        event.consume();
        return;
    }

    if (key.pageModifier()) {
        return;
    }

    if (key.is(Key::Encoder) && selectedRow() == 0) {
        int index = _project.selectedTrackIndex();
        switch (_project.selectedTrack().trackMode()) {
            case Track::TrackMode::Note:
                _manager.pages().textInput.show("NAME:", _noteTrack->name(), NoteTrack::NameLength, [this, index] (bool result, const char *text) {
                    if (result) {
                        _project.selectedTrack().noteTrack().setName(text);
                        
                        _project.setSelectedTrackIndex(2);
                        _project.setSelectedTrackIndex(index);
                    }
                });
                break;
            case Track::TrackMode::Curve:
                _manager.pages().textInput.show("NAME:", _curveTrack->name(), CurveTrack::NameLength, [this] (bool result, const char *text) {
                    if (result) {
                        _project.selectedTrack().curveTrack().setName(text);
                    }
                });
                break;  
            case Track::TrackMode::MidiCv:
                _manager.pages().textInput.show("NAME:", _midiCvTrack->name(), MidiCvTrack::NameLength, [this] (bool result, const char *text) {
                    if (result) {
                        _project.selectedTrack().midiCvTrack().setName(text);
                    }
                });
                break;   
            case Track::TrackMode::Stochastic:
                _manager.pages().textInput.show("NAME:", _stochasticTrack->name(), StochasticTrack::NameLength, [this] (bool result, const char *text) {
                    if (result) {
                        _project.selectedTrack().stochasticTrack().setName(text);
                    }
                 });
            break;   
            case Track::TrackMode::Logic:
                _manager.pages().textInput.show("NAME:", _logicTrack->name(), LogicTrack::NameLength, [this] (bool result, const char *text) {
                    if (result) {
                        _project.selectedTrack().logicTrack().setName(text);
                    }
                 });
            break;  
            case Track::TrackMode::Last:
                break;     
        }

    return;
}


    ListPage::keyPress(event);
}

void TrackPage::setTrack(Track &track) {
    RoutableListModel *newListModel = _listModel;

    switch (track.trackMode()) {
    case Track::TrackMode::Note:
        _noteTrackListModel.setTrack(track.noteTrack());
        newListModel = &_noteTrackListModel;
        _noteTrack = &track.noteTrack();
        break;
    case Track::TrackMode::Curve:
        _curveTrackListModel.setTrack(track.curveTrack());
        newListModel = &_curveTrackListModel;
        _curveTrack = &track.curveTrack();
        break;
    case Track::TrackMode::MidiCv:
        _midiCvTrackListModel.setTrack(track.midiCvTrack());
        newListModel = &_midiCvTrackListModel;
        _midiCvTrack = &track.midiCvTrack();
        break;
    case Track::TrackMode::Stochastic:
        _stochasticTrackListModel.setTrack(track.stochasticTrack());
        newListModel = &_stochasticTrackListModel;
        _stochasticTrack = &track.stochasticTrack();
        break;
    case Track::TrackMode::Logic:
        _logicTrackListModel.setTrack(track.logicTrack());
        newListModel = &_logicTrackListModel;
        _logicTrack = &track.logicTrack();
        break;    
    case Track::TrackMode::Last:
        ASSERT(false, "invalid track mode");
        break;
    }

    if (newListModel != _listModel) {
        _listModel = newListModel;
        setListModel(*_listModel);
    }
}

void TrackPage::contextShow(bool doubleClick) {
    showContextMenu(ContextMenu(
        contextMenuItems,
        int(ContextAction::Last),
        [&] (int index) { contextAction(index); },
        [&] (int index) { return contextActionEnabled(index); },
        doubleClick
    ));
}

void TrackPage::contextAction(int index) {
    switch (ContextAction(index)) {
    case ContextAction::Init:
        initTrackSetup();
        break;
    case ContextAction::Copy:
        copyTrackSetup();
        break;
    case ContextAction::Paste:
        pasteTrackSetup();
        break;
    case ContextAction::Route:
        initRoute();
        break;
    case ContextAction::Last:
        break;
    }
}

bool TrackPage::contextActionEnabled(int index) const {
    switch (ContextAction(index)) {
    case ContextAction::Paste:
        return _model.clipBoard().canPasteTrack();
    case ContextAction::Route:
        return _listModel->routingTarget(selectedRow()) != Routing::Target::None;
    default:
        return true;
    }
}

void TrackPage::initTrackSetup() {
    _project.selectedTrack().clear();
    setTrack(_project.selectedTrack());
    showMessage("TRACK INITIALIZED");
}

void TrackPage::copyTrackSetup() {
    _model.clipBoard().copyTrack(_project.selectedTrack());
    showMessage("TRACK COPIED");
}

void TrackPage::pasteTrackSetup() {
    // we are about to change track engines -> lock the engine to avoid inconsistent state
    _engine.lock();
    _model.clipBoard().pasteTrack(_project.selectedTrack());
    _engine.unlock();
    setTrack(_project.selectedTrack());
    showMessage("TRACK PASTED");
}

void TrackPage::initRoute() {
    _manager.pages().top.editRoute(_listModel->routingTarget(selectedRow()), _project.selectedTrackIndex());
}
