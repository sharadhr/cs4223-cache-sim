# Domain Model
```
CacheFactory (switch between DRAGON and MESI)

Input file -> Instructions (LD/ST)

ProcessorPool

Processor ---------------
PC -> Cache(L1) -> CoreMonitor
---------------------------

Cache--------------------
CacheController -> Array -> Struct CacheLine{ Data, State }
-------------------------

Bus---------------
DataStore Queue[] -> BusMonitor
---------------------

BusMonitor---------
InvalidationCounter/Updates 
  -> Data traffic
------------------

CoreMonitor---------
Execution cycle -> Number of LD/ST inst -> Compute Cycles per core -> number of idle cycles
 -> Cache miss rate 
------------------

SystemMonitor---------
BusMonitor -> CoreMonitors
--------------------

```

# API
```
main {
  (processProol: ProcessPool).feed(fileName: )
  (processProol: ProcessPool).run()
}

ProcessorPool:
  processors: Processors[4]
  feed(filePath="") // setup
  run() {
    while (true) {
      if (blockCycles1 == 0) {
        blockCycles1 = procsesors[0].run1Inst(bus)
      }

      if (blockCycles2 == 0) {
        blockCycles2 = procsesors[1].run1Inst()
      }

      if (blockCycles3 == 0) {
        blockCycles3 = procsesors[2].run1Inst()
      }

      if (blockCycles4 == 0) {
        blockCycles4 = procsesors[3].run1Inst()
      }

      if (blockCycle1 == -1 && 
        blockCycle2 == -1 && 
        blockCycle3 == -1 && 
        blockCycle4 == -1) {
        break;
      }

      blockCycles1--;
      blockCycles2--;
      blockCycles3--;
      blockCycles4--;

    }
    cleanUp()
  }

  cleanUp() {
    systemMon = new SystemMonitor(busMonitor, coreMonitors);
    systemMon.summaryToCSV();
  }

struct Instruction {
  label: enum {Load, Store, Other}
  value: integer
}

Processor:
  instructions: Vec<Instruction>

  private pc: integer
  private inputStream: InstructionStream

  public coreMonitor: CoreMonitor

  getNextInstruction(): Instruction

  // pipeline functions
  private execute(inst: Instruction): integer
  private writeBack(inst: Instruction): integer

  private cacheController: CacheController
  private coreMonitor: CoreMonitor

  private currentInst: Instruction
  public run1Inst(): int {
    pc += execute(inst)
    pc += writeback(inst)
  }

abstract interface CacheController<Vec<>>:
  private bus: Bus&

  has(address: int32): boolean

  get(address: int32): <T> 
    check bus in the beginning change state of other caches

  put(address: int32): <T> 
    check bus in the beginning change state of other caches
    bus.setStates(states);

  setBus(bus: Bus&): void


processors[0].cacheController.put(x, 1)
 -> this.bus.setStates(x, [M, I, I , I])



=========== Protocol specific (MESI/Dragon)
Bus:
  private caches: Cache&[]
  busMonitor: BusMonitor

  setStates(cacheline, states)
    busMonitor.numInvalid += 3;
    busMonitor.numUpdate += 3;
    busMonitor.dataTraffic += cacheLineSize;
    busMonitor.numPublic: += numOfS
    busMonitor.numPrivate: += numOfME

BusMonitor:
  numInvalid: int
  numUpdate: int
  dataTrafficByes: int
  numPublic: int
  numPrivate: int

MESICache implements CacheController
DragonCache implements CacheController

MESIBus:
DragonBus:

CoreMonitor:
  numOfExecutionCycles
  numOfComputeCycles
```
