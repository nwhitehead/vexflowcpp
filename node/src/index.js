
import Blob from "cross-blob";
import FS from "fs";
import jsdom from "jsdom";
// Note that canvas is a dependency of jsdom
import express from 'express';
import OSMD from '../opensheetmusicdisplay.debug.cjs';

function sleep (ms) {
    return new Promise((resolve) => {
        setTimeout(resolve, ms);
    });
}

function debug(txt) {
    console.log(txt);
}

// global variables
const sampleFilename = 'sample/BrookeWestSample.musicxml';
const pageWidth = 1600;
const pageHeight = 1200;
const endlessPage = pageWidth == 0 || pageHeight == 0;
const pageFormat = endlessPage ? 'Endless' : `${pageWidth}x${pageHeight}`;
const imageFormat = 'png';
const useWhiteTabNumberBackground = true;
// use white instead of transparent background for tab numbers for PNG export.
//   can fix black rectangles displayed, depending on your image viewer / program.
//   though this is unnecessary if your image viewer displays transparent as white

//////////////////////////////////////////////////////////////////////////////
// BEGIN Browser backend stubs
//////////////////////////////////////////////////////////////////////////////
// eslint-disable-next-line @typescript-eslint/no-var-requires
const dom = new jsdom.JSDOM("<!DOCTYPE html></html>");
// eslint-disable-next-line no-global-assign
// window = dom.window;
// eslint-disable-next-line no-global-assign
// document = dom.window.document;
// eslint-disable-next-line no-global-assign
global.window = dom.window;
// eslint-disable-next-line no-global-assign
global.document = window.document;
global.HTMLElement = window.HTMLElement;
global.HTMLAnchorElement = window.HTMLAnchorElement;
global.XMLHttpRequest = window.XMLHttpRequest;
global.DOMParser = window.DOMParser;
global.Node = window.Node;
    // fix Blob not found (to support external modules like is-blob)
global.Blob = Blob;
const div = document.createElement("div");
div.id = "browserlessDiv";
document.body.appendChild(div);
const zoom = 1.0;
// width of the div / PNG generated
let width = pageWidth * zoom;
if (endlessPage) {
    width = 1440;
}
let height = pageHeight;
if (endlessPage) {
    height = 32767;
}
div.width = width;
div.height = height;
div.setAttribute("width", width);
div.setAttribute("height", height);
div.setAttribute("offsetWidth", width);
Object.defineProperties(window.HTMLElement.prototype, {
    offsetLeft: {
        get: function () { return parseFloat(window.getComputedStyle(this).marginTop) || 0; }
    },
    offsetTop: {
        get: function () { return parseFloat(window.getComputedStyle(this).marginTop) || 0; }
    },
    offsetHeight: {
        get: function () { return height; }
    },
    offsetWidth: {
        get: function () { return width; }
    }
});
//////////////////////////////////////////////////////////////////////////////
// END Browser backend stubs
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// OSMD configuration
//////////////////////////////////////////////////////////////////////////////

console.log(div.width, div.height);
const backend = imageFormat === 'png' ? 'canvas' : 'svg';
const osmdInstance = new OSMD.OpenSheetMusicDisplay(div, {
    autoResize: false,
    backend: backend,
    pageBackgroundColor: '#eeeeee',
    pageFormat: pageFormat,
    // drawFromMeasureNumber: isFunctionTestDrawingRange ? 9 : 1,
    // drawUpToMeasureNumber: isFunctionTestDrawingRange ? 12 : Number.MAX_SAFE_INTEGER,
    // darkMode: true,
});

if (useWhiteTabNumberBackground && backend === 'png') {
    osmdInstance.EngravingRules.pageBackgroundColor = '#FFFFFF';
}
osmdInstance.setLogLevel('debug');
debug(`osmd PageFormat: ${osmdInstance.EngravingRules.PageFormat.idString}`);
debug(`osmd PageHeight: ${osmdInstance.EngravingRules.PageHeight}`);

let loadParameter = FS.readFileSync(sampleFilename);

if (sampleFilename.endsWith('.mxl')) {
    loadParameter = await OSMD.MXLHelper.MXLtoXMLstring(loadParameter);
} else {
    loadParameter = loadParameter.toString();
}
debug(`loadParameter.length: ${loadParameter.length}`);

await osmdInstance.load(loadParameter, sampleFilename); 
debug('MusicXML content loaded');

await osmdInstance.render();
debug('MusicXML content rendered');

let dataURLs = [];

for (let pageNumber = 1; pageNumber < Number.POSITIVE_INFINITY; pageNumber++) {
    if (imageFormat === "png") {
        const canvasImage = document.getElementById("osmdCanvasVexFlowBackendCanvas" + pageNumber);
        if (!canvasImage) {
            break;
        }
        dataURLs.push(canvasImage.toDataURL());
    }
}

debug(`MusicXML got data URLs`);

const imageBuffer = Buffer.from(dataURLs[0].split(";base64,").pop(), "base64");

debug('Computed imageBuffer');

const app = express();

app.get('/png', (req, res) => {
    res.contentType('image/png');
    res.send(imageBuffer);
});

app.get('/', (req, res) => {
    res.send('Hello world from the server');
});

const PORT = 5000;
 
app.listen(PORT, () => {
    console.log(`Running on PORT ${PORT}`);
});
