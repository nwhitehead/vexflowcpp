import { Canvas } from '../src/vexflow_wrap.mjs';
import '../external/vexflow-debug.js.js';

const VF = window.VexFlow;

cpp_register_font('../external/Bravura.otf', 'Bravura');

const width = 1024;
const height = 768

export async function main() {

    const canvas = new Canvas(width, height);
    const ctx = VF.Renderer.buildContext(canvas, 1, width, height);

    const stave = new VF.Stave(10, 40, 400);
    stave.addClef("treble").addTimeSignature("4/4");
    stave.setContext(ctx).draw();

    // const { Factory, EasyScore, System } = VF;
    // const vf = new Factory({
    //     renderer: { elementId: canvas, width, height, backend: 1 },
    // });
    // // Script does not have lexical scope so can't see the const vf, so expose it globally.
    // globalThis.vf = vf;
    // globalThis.VF = VF;
    // globalThis.context = ctx;
    // cpp_import_script('../test/test0.js');
    // vf.draw();
}

await main();
