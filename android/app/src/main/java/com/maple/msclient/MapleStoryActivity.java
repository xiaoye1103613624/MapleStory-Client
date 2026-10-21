package com.maple.msclient;

import org.libsdl.app.SDLActivity;

/**
 * MapleStory (BeiDou gms083) Android entry activity.
 *
 * All native code (the client, SDL3, freetype, lz4, squish and the nlnx
 * reader) is statically linked into a single libmain.so, so we only need to
 * load that one shared library. The default SDLActivity.getLibraries() tries to
 * load "SDL3" first, which does not exist as a separate .so in our build, so
 * we override it here.
 */
public class MapleStoryActivity extends SDLActivity {
    @Override
    protected String[] getLibraries() {
        return new String[] {
            "main"
        };
    }
}
