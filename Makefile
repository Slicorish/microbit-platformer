# Edit this line to where your micro:bit device is mounted:
MICROBIT_PATH = /run/media/billy/MICROBIT

# Uncomment the line below if you are using the supplied Ubuntu VM.
#MICROBIT_PATH = /media/student/MICROBIT

SOURCE_FILES += \
    source/ByteBuf.cpp \
    source/ByteBuf.h \
    source/ConnectGameState.cpp \
    source/ConnectGameState.h \
    source/DeathGameState.cpp \
    source/DeathGameState.h \
    source/Game.cpp \
    source/Game.h \
    source/GameState.h \
    source/main.cpp \
    source/MenuGameState.cpp \
    source/MenuGameState.h \
    source/Player.cpp \
    source/Player.h \
    source/PlayGameState.cpp \
    source/PlayGameState.h \
    source/Vector2i.cpp \
    source/Vector2i.h \
    source/World.cpp \
    source/World.h

OUTPUT_FILE = build/bbc-microbit-classic-gcc/source/microbit-platformer-combined.hex

run: ${OUTPUT_FILE}
	cp ${OUTPUT_FILE} ${MICROBIT_PATH}
	cp ${OUTPUT_FILE} ${MICROBIT_PATH}1

${OUTPUT_FILE}: ${SOURCE_FILES}
	yt build
