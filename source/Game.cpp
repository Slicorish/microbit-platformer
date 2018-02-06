#include "Game.h"
#include "MenuGameState.h"

namespace platformer {

    Game::Game() = default;

    Game::~Game() {
        delete microBit;
        delete screen;
        delete state;
    }

    void Game::run() {
        // Game should start in the main menu.
        setState(new MenuGameState(this));
        microBit->display.print("B");

        // Initialize the micro:bit.
        microBit->init();
        microBit->display.setDisplayMode(DISPLAY_MODE_GREYSCALE);

        // Register event handlers.
        microBit->messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, this, &Game::onButtonAPress);
        microBit->messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, this, &Game::onButtonBPress);
        microBit->messageBus.listen(MICROBIT_ID_BUTTON_AB, MICROBIT_BUTTON_EVT_CLICK, this, &Game::onButtonABPress);
        microBit->messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, this, &Game::onMessage);

        // Now enter the game loop.
        while (state != nullptr) {
            changingState = false;
            state->run();
        }
    }

    void Game::onButtonAPress(MicroBitEvent) {
        microBit->display.print("A");
//        if (!changingState) {
//            state->onButtonAPress();
//        }
    }

    void Game::onButtonBPress(MicroBitEvent) {
        if (!changingState) {
            state->onButtonBPress();
        }
    }

    void Game::onButtonABPress(MicroBitEvent) {
        if (!changingState) {
            state->onButtonABPress();
        }
    }

    void Game::onMessage(MicroBitEvent) {
        // Create byte buffer for easy message reading.
        PacketBuffer received = microBit->radio.datagram.recv();
        ByteBuf in;
        for (int i = 0; i < received.length(); i++) {
            in.write(received.getByte(i));
        }

        // Read game ID, if message is not for this game then ignore.
        int gameId = in.readInt();
        if (gameId != GAME_ID) {
            return;
        }

        // Read the sender and target IDs.
        auto senderId = (uint32_t) in.readInt();
        auto targetId = (uint32_t) in.readInt();

        // Attempt to setup a new connection with whoever sent this packet, so
        // long as we are currently not already connected to any other devices.
        if (!isConnected()) {
            PacketType packetType = in.readPacketType();
            setupConnection(packetType, senderId, targetId);
            return;
        }

        // Ignore messages from non-partnered devices.
        if (senderId != getPartnerId()) {
            return;
        }

        // Disconnect if partner is found communicating with others.
        if (targetId != getId()) {
            disconnect();
            return;
        }

        // Wait until state has changed.
        while (isChangingState()) {
            getMicroBit()->sleep(1);
        }

        // Inform the current running state of this message.
        getState()->onMessage(in);
    }

    void Game::setupConnection(PacketType packetType, uint32_t senderId, uint32_t targetId) {
        // Begin writing response.
        ByteBuf out;
        out.writeInt(GAME_ID);

        switch (packetType) {
            case BROADCAST: {
                // Broadcasts must have a target ID of zero.
                if (targetId != 0) {
                    return;
                }

                // Write partner request.
                out.writeInt(getId());      // Sender ID.
                out.writeInt(senderId);     // Target ID.
                out.writePacketType(PARTNER_REQUEST);
                break;
            }

            case PARTNER_REQUEST: {
                // Ignore partner requests if they're not meant for us.
                if (targetId != getId()) {
                    return;
                }

                // Write another partner request to accept.
                out.writeInt(getId());      // Sender ID.
                out.writeInt(senderId);     // Target ID.
                out.writePacketType(PARTNER_REQUEST);

                // Update connection info.
                setConnected(true);
                setPartnerId(senderId);

                // Switch to the menu game state.
                auto *nextState = new MenuGameState(this);
                setState(nextState);
                break;
            }

            default: {
                // Ignore invalid packets for this state.
                return;
            }
        }

        // On successful interpretation and a response is written, send it.
        sendPacket(out);
    }

    ByteBuf Game::createPacket() {
        ByteBuf buf;
        buf.writeInt(GAME_ID);      // Game ID
        buf.writeInt(id);           // Sender ID
        buf.writeInt(partnerId);    // Target ID
        return buf;
    }

    void Game::disconnect() {
        if (!isMultiplayer()) {
            return;
        }

        // Send disconnect packet.
        ByteBuf out = createPacket();
        out.writePacketType(PacketType::DISCONNECT);
        sendPacket(out);

        // Update game state, disabling connection and the radio.
        getMicroBit()->radio.disable();
        setMultiplayer(false);
        setConnected(false);
        setPartnerId(0);
    }

    void Game::sendPacket(ByteBuf packet) {
        microBit->radio.datagram.send(packet.data(), (int) packet.size());
    }

    MicroBit *Game::getMicroBit() const {
        return microBit;
    }

    MicroBitImage *Game::getScreen() const {
        return screen;
    }

    GameState *Game::getState() const {
        return state;
    }

    void Game::setState(GameState *state) {
        changingState = true;
        Game::state = state;
    }

    bool Game::isChangingState() const {
        return changingState;
    }

    bool Game::isMultiplayer() const {
        return multiplayer;
    }

    void Game::setMultiplayer(bool multiplayer) {
        Game::multiplayer = multiplayer;
    }

    bool Game::isConnected() const {
        return connected;
    }

    void Game::setConnected(bool connected) {
        Game::connected = connected;
    }

    uint32_t Game::getId() const {
        return id;
    }

    uint32_t Game::getPartnerId() const {
        return partnerId;
    }

    void Game::setPartnerId(uint32_t partnerId) {
        Game::partnerId = partnerId;
    }

}
