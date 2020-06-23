var test = require("test");

var fs = require('fs');
var path = require('path');
var coroutine = require('coroutine');
var uuid = require('uuid');

var { rmFile } = require('./_helpers/fs')
var { ensureDirectoryExisted } = require('./_helpers/process');

test.setup();

describe('fs.watch*', () => {
    before(() => {
        rmFile(path.resolve(__dirname, './fswatch_files'));
        ensureDirectoryExisted(path.resolve(__dirname, './fswatch_files'));
    })
    describe.skip("manual watch", () => {
        it("try not quit", () => {
            let count = 0;
            let evt = new coroutine.Event();
            const watcher = fs.watch(
                './test/fswatch_files/nogit-manual.txt',
                (evt_type, filename) => {
                    console.log('观察到了 evt_type: %s; filename: %s', evt_type, filename)
                    if (++count > 10) {
                        watcher.close()
                        evt.set()
                    }
                }
            )

            evt.wait()
            assert.ok(evt.isSet())
        });
    })
    const basedir = path.resolve(__dirname)
    const relcwdbase = path.relative(process.cwd(), basedir)
    var TRIGGER_TIME = 3
    var writeFile = (rel, content = '') => {
        fs.writeTextFile(path.resolve(rel), content);
    }
    var resolve_reltocwd = (rel) => {
        return path.join(relcwdbase, rel)
    }
    var createFile = (rel, content = '') => {
        writeFile(rel, content);
    }
    var delFile = (rel) => {
        fs.unlink(rel)
    }

    describe("write file", () => {
        var proc = ({
            changeEventSource = '',
            next,
        }) => {
            var _uuid = uuid.snowflake().hex()
            var relfile = resolve_reltocwd(`./fswatch_files/nogit-${_uuid}.txt`)

            writeFile(relfile, '')

            var watcher
            var changeTriggerCount = 0
            const _handler = (evtType, filename) => {
                assert.isString(evtType)
                assert.isString(filename)
                console.log('\twatched file change, type: %s; filename: %s', evtType, filename)

                changeTriggerCount++;
            }
            switch (changeEventSource) {
                case 'listener':
                    watcher = fs.watch(relfile, _handler)
                    break
                case 'register':
                    watcher = fs.watch(relfile)
                    watcher.on('change', _handler)
                    break
            }
            let writeCount = 0
            var noTriggerAfterClose = true
            const itv = setInterval(() => {
                if (writeCount < TRIGGER_TIME) {
                    writeFile(relfile, `this is: ${++writeCount}`)
                    return;
                }

                clearInterval(itv);
                watcher.close();
                watcher.on('change', () => {
                    noTriggerAfterClose = false;
                });
                /**
                 * in some system, too short file change duration would collapse to one-time, but the duration
                 * is much less than 1500ms
                 */
            }, 1500);

            watcher.on('close', () => {
                assert.ok(changeTriggerCount > 0);
                assert.ok(changeTriggerCount >= writeCount);

                assert.isTrue(noTriggerAfterClose);

                next();
            })

            assert.isObject(watcher)
            assert.isFunction(watcher.close)
        }

        it(`event: 'change', from listener`, (next) => {
            proc({ changeEventSource: 'listener', next });
        });

        it(`event: 'change', from register`, (next) => {
            proc({ changeEventSource: 'register', next });
        });
    });

    describe("watch Directory create file", () => {
        var proc = ({
            changeEventSource = '',
            next,
        }) => {
            var _uuid = uuid.snowflake().hex()
            var reldir = resolve_reltocwd(`./fswatch_files/${_uuid}`)

            ensureDirectoryExisted(reldir)

            var watcher
            var changeTriggerCount = 0
            const _handler = (evtType, filename) => {
                assert.isString(evtType)
                assert.isString(filename)
                console.log('\twatched file change, type: %s; filename: %s', evtType, filename)

                changeTriggerCount++;
            }
            switch (changeEventSource) {
                case 'listener':
                    watcher = fs.watch(reldir, _handler)
                    break
                case 'register':
                    watcher = fs.watch(reldir)
                    watcher.on('change', _handler)
                    break
            }
            let writeCount = 0
            var noTriggerAfterClose = true
            const itv = setInterval(() => {
                if (writeCount < TRIGGER_TIME) {
                    createFile(path.join(reldir, `nogit-${writeCount}.txt`), `this is: ${++writeCount}`)
                    return;
                }

                clearInterval(itv);
                watcher.close();
                watcher.on('change', () => {
                    noTriggerAfterClose = false;
                });
                /**
                 * in some system, too short file change duration would collapse to one-time, but the duration
                 * is much less than 200ms
                 */
            }, 200);

            watcher.on('close', () => {
                assert.ok(changeTriggerCount > 0);
                assert.ok(changeTriggerCount >= writeCount);

                assert.isTrue(noTriggerAfterClose);

                next();
            })
        }

        it(`event: 'rename', from listener`, (next) => {
            proc({ changeEventSource: 'listener', next });
        });

        it(`event: 'rename', from register`, (next) => {
            proc({ changeEventSource: 'register', next });
        });
    });

    describe("watch Directory delete file", () => {
        var proc = ({
            changeEventSource = '',
            next,
        }) => {
            var _uuid = uuid.snowflake().hex()
            var reldir = resolve_reltocwd(`./fswatch_files/${_uuid}`)
            ensureDirectoryExisted(reldir)

            var files = Array(TRIGGER_TIME)
                .fill(undefined)
                .map((_, c) => {
                    var fname = path.resolve(reldir, `nogit-${c}.txt`)
                    writeFile(fname, `this is file to delete, ${c}`);
                    return fname
                })
            var fileCount = files.length


            var watcher
            var changeTriggerCount = 0
            const _handler = (evtType, filename) => {
                assert.isString(evtType)
                assert.isString(filename)
                console.log('\twatched file change, type: %s; filename: %s', evtType, filename)

                changeTriggerCount++;
            }
            switch (changeEventSource) {
                case 'listener':
                    watcher = fs.watch(reldir, _handler)
                    break
                case 'register':
                    watcher = fs.watch(reldir)
                    watcher.on('change', _handler)
                    break
            }

            var noTriggerAfterClose = true
            const itv = setInterval(() => {
                if (files.length) {
                    delFile(files.shift())
                    return;
                }

                clearInterval(itv);
                watcher.close();
                watcher.on('change', () => {
                    noTriggerAfterClose = false;
                });
            }, 1500);

            watcher.on('close', () => {
                assert.ok(changeTriggerCount > 0);
                assert.ok(changeTriggerCount >= fileCount);

                assert.isTrue(noTriggerAfterClose);

                next();
            })
        }

        it(`event: 'rename', from listener`, (next) => {
            proc({ changeEventSource: 'listener', next });
        });

        it(`event: 'rename', from register`, (next) => {
            proc({ changeEventSource: 'register', next });
        });
    });
})

require.main === module && test.run(console.DEBUG);