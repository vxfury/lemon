/**
 * This is the tests for the MPSC CircularFIFO
 */
//	System Headers
#include <iostream>
#include <string>

//	Third-Party Headers

//	Other Headers
#include "mpsc/CircularFIFO.h"
#include "util/timer.h"
#include "hammer.h"
#include "drain.h"

int main(int argc, char *argv[])
{
    bool error = false;

    // make a circular FIFO of 1024 int32_t values - max
    dkit::mpsc::CircularFIFO<int32_t, 10> q;
    // put 500 values on the queue - and check the size
    if (!error) {
        std::cout << "=== Testing speed and correctness of CircularFIFO ===" << std::endl;

        // get the starting time
        uint64_t goTime = dkit::util::timer::usecStamp();

        // do this often enough to rotate through the size of the values
        int32_t trips = 100000;
        for (int32_t cycle = 0; cycle < trips; ++cycle) {
            // put 500 values on the queue - and check the size
            for (int32_t i = 0; i < 500; ++i) {
                if (!q.push(i)) {
                    error = true;
                    std::cout << "ERROR - could not push the value " << i << std::endl;
                    break;
                }
            }
            // now check the size
            if (!error) {
                if (q.size() != 500) {
                    error = true;
                    std::cout << "ERROR - pushed 500 integers, but size() reports only " << q.size() << std::endl;
                } else {
                    if (cycle == 0) {
                        std::cout << "Passed - pushed on 500 integers" << std::endl;
                    }
                }
            }
            // pop off 500 integers and it should be empty
            if (!error) {
                int32_t v = 0;
                for (int32_t i = 0; i < 500; ++i) {
                    if (!q.pop(v) || (v != i)) {
                        error = true;
                        std::cout << "ERROR - could not pop the value " << i << std::endl;
                        break;
                    }
                }
            }
            // now check the size
            if (!error) {
                if (!q.empty()) {
                    error = true;
                    std::cout << "ERROR - popped 500 integers, but size() reports " << q.size() << std::endl;
                } else {
                    if (cycle == 0) {
                        std::cout << "Passed - popped all 500 integers" << std::endl;
                    }
                }
            }
            // now make sure we can't pop() anything
            if (!error) {
                int32_t v = 0;
                if (!q.pop(v)) {
                    if (cycle == 0) {
                        std::cout << "Passed - unable to pop from an empty queue" << std::endl;
                    }
                } else {
                    error = true;
                    std::cout << "ERROR - popped " << v << " from an empty queue - shouldn't be possible" << std::endl;
                }
            }
        }

        // get the elapsed time
        goTime = dkit::util::timer::usecStamp() - goTime;
        std::cout << "Passed - did " << (trips * 500) << " push/pop pairs in " << (goTime / 1000.0)
                  << "ms = " << ((goTime * 1000.0) / (trips * 500.0)) << "ns/op" << std::endl;
    }

    // check on how the crash recovery works
    if (!error) {
        // make sure it's all cleared out for this test
        q.clear();

        std::cout << "=== Testing crash surviveability CircularFIFO ===" << std::endl;

        // push values starting at 0 up until we can't push any more
        int32_t v = 0;
        int32_t lim = 0;
        while (q.push(lim)) {
            ++lim;
        }
        std::cout << "Passed - Failed on pushing " << lim << std::endl;
        for (int32_t i = 0; i < lim; ++i) {
            if (!q.pop(v) || (v != i)) {
                error = true;
                std::cout << "ERROR - could not pop the value " << i << std::endl;
                break;
            }
        }
        if (!error) {
            std::cout << "Passed - after crash, still able to recover all values" << std::endl;
        }
    }

    /**
     * Make a set of Hammers and a Drain and test threading
     */
    if (!error) {
        Hammer *src[] = {NULL, NULL, NULL, NULL};
        for (uint32_t i = 0; i < 4; ++i) {
            if ((src[i] = new Hammer(i, &q, 250)) == NULL) {
                std::cout << "PROBLEM - unable to make Hammer #" << i << "!" << std::endl;
                break;
            }
        }
        Drain dest(0, &q);
        // now start the drain then all the hammers
        dest.start();
        for (uint32_t i = 0; i < 4; ++i) {
            if (src[i] != NULL) {
                src[i]->start();
            }
        }
        // now let's wait for all the hammers to be done
        bool allSent = false;
        while (!allSent) {
            // assume done, but check for the first failure
            allSent = true;
            // now let's check all the hammers to see if they are done
            for (uint32_t i = 0; i < 4; ++i) {
                if (src[i] != NULL) {
                    if (!src[i]->isDone()) {
                        allSent = false;
                        break;
                    }
                }
            }
            // see if we need to wait a bit to try again
            if (!allSent) {
                usleep(250000);
            }
        }
        // now tell the drain to stop when the queue is empty
        dest.stopOnEmpty();
        while (!dest.isDone()) {
            usleep(250000);
        }
        // now let's see what we have
        uint32_t cnt = dest.getCount();
        if (cnt == 1000) {
            std::cout << "Passed - popped " << cnt << " integers, from four hammer threads" << std::endl;
        } else {
            std::cout << "ERROR - popped " << cnt << " integers, but should have popped " << 1000 << std::endl;
        }
        // finally, clean things up
        for (uint32_t i = 0; i < 4; ++i) {
            if (src[i] != NULL) {
                delete src[i];
                src[i] = NULL;
            }
        }
    }

    std::cout << (error ? "FAILED!" : "SUCCESS") << std::endl;
    return (error ? 1 : 0);
}
