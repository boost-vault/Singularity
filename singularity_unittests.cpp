//----------------------------------------------------------------------------
//! \brief
//----------------------------------------------------------------------------
#define BOOST_TEST_MAIN defined
#include <boost/test/unit_test.hpp>
#include <boost/noncopyable.hpp>
#include <singularity.hpp>

namespace {

using ::boost::singularity;
using ::boost::single_threaded;
using ::boost::multi_threaded;

// A generic, non POD class.
class Event : private boost::noncopyable {
public:
    Event() {}
private:
};

// This class demonstrates making itself a Singularity,
// by making its constructors private, and friending
// the Singulariy using the convenience macro.
class Horizon : private boost::noncopyable {
private:
    Horizon()                 : mInt(0),    mEvent(), mEventPtr(&mEvent),   mEventRef(mEvent)    {}
    Horizon(int xInt)         : mInt(xInt), mEvent(), mEventPtr(&mEvent),   mEventRef(mEvent)    {}
    Horizon(Event* xEventPtr) : mInt(0),    mEvent(), mEventPtr(xEventPtr), mEventRef(mEvent)    {}
    Horizon(Event& xEventRef) : mInt(0),    mEvent(), mEventPtr(&mEvent),   mEventRef(xEventRef) {}
    Horizon(int xInt, Event* xEventPtr, Event& xEventRef)
                              : mInt(xInt), mEvent(), mEventPtr(xEventPtr), mEventRef(xEventRef) {}
    int    mInt;
    Event  mEvent;
    Event* mEventPtr;
    Event& mEventRef;

    FRIEND_CLASS_SINGULARITY;
};

BOOST_AUTO_TEST_CASE(shouldThrowOnDoubleCalls) {
    Horizon & horizon = singularity<Horizon>::create();
    (void)horizon;
    BOOST_CHECK_THROW( // Call create() twice in a row
        Horizon & horizon2 = singularity<Horizon>::create(),
        boost::singularity_already_created
    );

    singularity<Horizon>::destroy();
    BOOST_CHECK_THROW( // Call destroy() twice in a row
        singularity<Horizon>::destroy(),
        boost::singularity_already_destroyed
    );
}

BOOST_AUTO_TEST_CASE(shouldThrowOnDoubleCallsWithDifferentArguments) {
    Horizon & horizon = singularity<Horizon>::create();
    (void)horizon;
    BOOST_CHECK_THROW( // Call create() twice in a row
        Horizon & horizon2 = (singularity<Horizon, single_threaded, int>::create(5)),
        boost::singularity_already_created
    );

    singularity<Horizon>::destroy();
    BOOST_CHECK_THROW( // Call destroy() twice in a row
        (singularity<Horizon, single_threaded, int>::destroy()),
        boost::singularity_already_destroyed
    );
}

BOOST_AUTO_TEST_CASE(shouldThrowOnDestroyWithWrongThreading) {
    Horizon & horizon = singularity<Horizon, single_threaded>::create();
    (void)horizon;

    BOOST_CHECK_THROW( // Call destroy() with wrong threading
        (singularity<Horizon, multi_threaded>::destroy()),
        boost::singularity_destroy_on_incorrect_threading
    );
    singularity<Horizon, single_threaded>::destroy();
}

BOOST_AUTO_TEST_CASE(shouldCreateDestroyCreateDestroy) {
    Horizon & horizon = singularity<Horizon>::create();
    (void)horizon;
    singularity<Horizon>::destroy();
    Horizon & new_horizon = singularity<Horizon>::create();
    (void)new_horizon;
    singularity<Horizon>::destroy();
}

BOOST_AUTO_TEST_CASE(useMultiThreadedPolicy) {
    Horizon & horizon = singularity<Horizon, multi_threaded>::create();
    (void)horizon;
    singularity<Horizon, multi_threaded>::destroy();
}

BOOST_AUTO_TEST_CASE(passOneArgumentByValue) {
    Horizon & horizon = singularity<Horizon, single_threaded, int>::create(3);
    (void)horizon;
    singularity<Horizon, single_threaded, int>::destroy();
}

BOOST_AUTO_TEST_CASE(passOneArgumentByAddress) {
    Event event;
    Horizon & horizon = singularity<Horizon, single_threaded, Event*>::create(&event);
    (void)horizon;
    singularity<Horizon, single_threaded, Event*>::destroy();
}

BOOST_AUTO_TEST_CASE(passOneArgumentByReference) {
    Event event;
    Horizon & horizon = singularity<Horizon, single_threaded, Event&>::create(event);
    (void)horizon;
    singularity<Horizon, single_threaded, Event&>::destroy();
}

BOOST_AUTO_TEST_CASE(passThreeArguments) {
    Event event;
    typedef singularity<Horizon, single_threaded, int, Event*, Event&> singularityType;
    Horizon & horizon = singularityType::create(3, &event, event);
    (void)horizon;
    singularityType::destroy();
}

} // namespace anonymous
