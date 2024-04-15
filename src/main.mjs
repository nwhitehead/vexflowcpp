 cpp_draw_character(0xe050, 0, 100);

import _ from '../src/wrap.mjs';

async function main() {
    cpp_import_script('../external/opensheetmusicdisplay.js');

    let osmd = new opensheetmusicdisplay.OpenSheetMusicDisplay("osmdContainer");
    osmd.setOptions({
        backend: "canvas",
        drawTitle: true,
    });
    console.log('ok');
    const song = cpp_read_file('../external/MozaVeilSample.xml');
    console.log(song.length);
    console.log(song.substr(0, 10));
    console.log(song.substr(-10));
    await osmd.load(song);
    console.log('Song loaded');
}

try {
    await main();
} catch(e) {
    console.log('Uncaught exception: ' + e);
    console.log(e.stack);
}
//   osmd
//     .load("MozaVeilSample.xml")
