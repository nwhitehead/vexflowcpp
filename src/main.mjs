 cpp_draw_character(0xe050, 0, 100);

import _ from '../src/wrap.mjs';
import { DOMParser, XMLSerializer } from '../external/xmldom.0.8.10.esm.js';
//import { opensheetmusicdisplay } from '../external/opensheetmusicdisplay.js';

globalThis.DOMParser = DOMParser;

async function main() {
    //cpp_import_script('../external/fxparser.min.js.js');
    cpp_import_script('../external/opensheetmusicdisplay.js');

    let osmd = new opensheetmusicdisplay.OpenSheetMusicDisplay("osmdContainer");
    osmd.setLogLevel('debug');
    osmd.setOptions({
        backend: "canvas",
        drawTitle: true,
    });
    console.log('ok');
    const song = cpp_read_file('../external/MozaVeilSample.xml');
    console.log(song.length);
    console.log(song.substr(0, 10));
    console.log(song.substr(-10));
    const parser = new DOMParser();
    const parsed = parser.parseFromString(song, 'application/xml');
    //console.log(parsed.childNodes[0].childNodes.length);
    const back = new XMLSerializer().serializeToString(parsed);
    console.log(`Length of serialized xml is ${back.length}`);
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
