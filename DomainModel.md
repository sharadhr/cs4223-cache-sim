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
      blockCycles1 = procsesors[0].runOneCycle(bus)
      blockCycles2 = procsesors[1].runOneCycle()
      blockCycles3 = procsesors[2].runOneCycle()
      blockCycles4 = procsesors[3].runOneCycle()
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

  private cycleCount: integer
  private inputStream: InstructionStream

  public coreMonitor: CoreMonitor

  getNextInstruction(): Instruction

  pipelineState: PipelineState

  // pipeline functions
  private execute(inst: Instruction): integer
  private writeBack(inst: Instruction): integer

  private cacheController: CacheController
  private coreMonitor: CoreMonitor

  private currentInst: Instruction

  private done: boolean

  public runOneCycle(): int {
    auto exit = pipelineState.update();
    if (exit) {
      this.currentInst = getNextInstruction();
    }

    if (currentInst.type == DONE) {
      this.done = true;
    }
  }

abstract interface CacheController<Vec<>>:
  private bus: Bus&

  private:
  has(address: int32): 
  
  public:
  get(address: int32): int32 
    check bus in the beginning change state of other caches
    return number of cycles elapsed

  put(address: int32): int32 
    check bus in the beginning change state of other caches
    bus.setStates(states)
    return number of cycles elapsed

  setBus(bus: Bus&): void


processors[0].cacheController.put(x, 1)
 -> this.bus.setStates(x, [M, I, I , I])



=========== Protocol specific (MESI/Dragon)
BusController<T>:
  private caches: Cache&[]
  busMonitor: BusMonitor

  setStates(cacheline, states: T])
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

MESIBus implements BusController
DragonBus implements BusController

CoreMonitor:
  numOfExecutionCycles
  numOfComputeCycles
```

# Bus Cache Interactions
```
"""
Check if other caches have this address are are waiting for it and return cycles
based on that:
1. Other cache already have valid adress: return cycles equal to 2N where N is number of words
(MESI) or 2 cycles for word (Dragon BusUpd only updates word).
2. Other caches are also waiting for the same data: return min(other cache's blockedFor + time required to
send from that cache to the requesting cache (see option 1), 100)
"""
Bus::cyclesToWait(pid, address) 

"""
Run at the end of the wait period of cache block and looks at below fields:
bool isBlocked{false};
uint32_t blockedFor{};
uint32_t blockedOnAddress{};
CacheOp blockOperation{CacheOp::PR_RD_MISS};

Based on blockOperation, issue a bus function to update the states for other caches:
P1: E------------------------------------------------> S
P2: I ---wait period---> Cache::issueBusTransaction(): S

And based on the decided states, inserts a new line with a new state in the cache.
"""
Cache::issueBusTransaction()

Cache::prRd(address) ---RdHit--> Cache::block(address, 1) ---wait for those cycles--> Cache::issueBusTransaction()
Cache::prRd(address) ---RdMiss--> Bus::cyclesToWait() ---> Cache::block(address, blockedFor, RD_MISS) ---wait for those cycles--> Cache::issueBusTransaction()
Cache::prWr(address) ---WrHit--> Cache::block(address, 1) ---wait for those cycles--> Cache::issueBusTransaction()
Cache::prWr(address) ---WrMiss--> Bus::cyclesToWait(address, 1) ---> Cache::block(address, blockedFor, WR_MISS) ---wait for those cycles--> Cache::issueBusTransaction()

```

## Assumptions:
```
1. Decode and issue is instant
2. No pipelining
3. Bus transactions after getting the data
4. Intermediate bus transactions can cause short circuiting
5. Snooping on blocknum
```
