import { Canvas } from '../src/vexflow_wrap.mjs';

cpp_register_font('../external/Bravura.otf', 'Bravura');
//cpp_register_font('../external/EBGaramond-VariableFont_wght.ttf', 'EBGaramond');
const scale = cpp_get_font_scale('Bravura', 150);

export async function main() {

    cpp_import_script('../external/vexflow-debug.js.js');

    const { Factory, EasyScore, System } = window.VexFlow;

    const canvas = new Canvas();
    
    const vf = new Factory({
        renderer: { elementId: canvas, width: 500, height: 200, backend: 1 },
    });
    
    //VexFlow.setMusicFont("Bravura");
    //"Petaluma", "Bravura", "Gonville");
    
    const score = vf.EasyScore();
    const system = vf.System();
    
    system
        .addStave({
            voices: [
                score.voice(score.notes('C5/q, B4, A4, G#4', { stem: 'up' })),
                score.voice(score.notes('C#4/h, C#4', { stem: 'down' })),        ],
        })
        .addClef('treble')
        .addTimeSignature('4/4');
    
    vf.draw();
}

await main();