globalThis.print = cpp_print;

globalThis.console = {
    log(txt) {
        print(txt);
    },
    warn(txt) {
        print(txt);
    },
    error(txt) {
        print(txt);
    },
};

globalThis.assert = function assert(value) {
    if (!value) {
        throw new Exception("Assertion failed");
    }
};

globalThis.window = {
    addEventListener() {},
    setTimeout() {},
};

export default {}
