#ifndef CUKE_HOOKREGISTRAR_HPP_
#define CUKE_HOOKREGISTRAR_HPP_

#include "Tag.hpp"
#include "../Scenario.hpp"
#include "../step/StepManager.hpp"

#include <boost/make_shared.hpp>
#include <boost/smart_ptr.hpp>
using boost::shared_ptr;

#include <list>

namespace cucumber {
namespace internal {

class CallableStep {
public:
    virtual void call() = 0;
};

class Hook {
public:
    void setTags(const std::string &csvTagNotation);
    virtual void invokeHook(Scenario *scenario, CallableStep *step);
    virtual void skipHook();
    virtual void body() = 0;
protected:
    bool tagsMatch(Scenario *scenario);

    template <typename Derived, typename R>
    static R invokeWithArgs(Derived& that, R (Derived::* f)()) {
        return (that.*f)();
    }
private:
    AndTagExpression tagExpression;
};

class BeforeHook : public Hook {
};

class AroundStepHook : public Hook {
public:
    virtual void invokeHook(Scenario *scenario, CallableStep *step);
    virtual void skipHook();
protected:
    CallableStep *step;
};

class AfterStepHook : public Hook {
};

class AfterHook : public Hook {
};

class UnconditionalHook : public Hook {
public:
    virtual void invokeHook(Scenario *scenario, CallableStep *step);
};

class BeforeAllHook : public UnconditionalHook {
};

class AfterAllHook : public UnconditionalHook {
};

class HookRegistrar {
public:
    typedef std::list< boost::shared_ptr<Hook> > hook_list_type;
    typedef std::list< boost::shared_ptr<AroundStepHook> > aroundhook_list_type;

    static void addBeforeHook(boost::shared_ptr<BeforeHook> afterHook);
    static void execBeforeHooks(Scenario *scenario);

    static void addAroundStepHook(boost::shared_ptr<AroundStepHook> aroundStepHook);
    static InvokeResult execStepChain(Scenario *scenario, const StepInfo* stepInfo, const InvokeArgs *pArgs);

    static void addAfterStepHook(boost::shared_ptr<AfterStepHook> afterStepHook);
    static void execAfterStepHooks(Scenario *scenario);

    static void addAfterHook(boost::shared_ptr<AfterHook> afterHook);
    static void execAfterHooks(Scenario *scenario);

    static void addBeforeAllHook(boost::shared_ptr<BeforeAllHook> beforeAllHook);
    static void execBeforeAllHooks();

    static void addAfterAllHook(boost::shared_ptr<AfterAllHook> afterAllHook);
    static void execAfterAllHooks();

private:
    static void execHooks(HookRegistrar::hook_list_type &hookList, Scenario *scenario);

protected:
    static hook_list_type& beforeAllHooks();
    static hook_list_type& beforeHooks();
    static aroundhook_list_type& aroundStepHooks();
    static hook_list_type& afterStepHooks();
    static hook_list_type& afterHooks();
    static hook_list_type& afterAllHooks();
};


class StepCallChain {
public:
    StepCallChain(Scenario *scenario, const StepInfo* stepInfo, const InvokeArgs *pStepArgs, HookRegistrar::aroundhook_list_type &aroundHooks);
    InvokeResult exec();
    void execNext();
private:
    void execStep();

    Scenario *scenario;
    const StepInfo* stepInfo;
    const InvokeArgs *pStepArgs;

    HookRegistrar::aroundhook_list_type::iterator nextHook;
    HookRegistrar::aroundhook_list_type::iterator hookEnd;
    InvokeResult result;
};

class CallableStepChain : public CallableStep {
public:
    CallableStepChain(StepCallChain *scc);
    void call();
private:
    StepCallChain *scc;
};


template<class T>
static int registerBeforeHook(const std::string &csvTagNotation) {
   boost::shared_ptr<T> hook(boost::make_shared<T>());
   hook->setTags(csvTagNotation);
   HookRegistrar::addBeforeHook(hook);
   return 0; // We are not interested in the ID at this time
}

template<class T>
static int registerAroundStepHook(const std::string &csvTagNotation) {
   boost::shared_ptr<T> hook(boost::make_shared<T>());
   hook->setTags(csvTagNotation);
   HookRegistrar::addAroundStepHook(hook);
   return 0;
}

template<class T>
static int registerAfterStepHook(const std::string &csvTagNotation) {
   boost::shared_ptr<T> hook(boost::make_shared<T>());
   hook->setTags(csvTagNotation);
   HookRegistrar::addAfterStepHook(hook);
   return 0;
}

template<class T>
static int registerAfterHook(const std::string &csvTagNotation) {
   boost::shared_ptr<T> hook(boost::make_shared<T>());
   hook->setTags(csvTagNotation);
   HookRegistrar::addAfterHook(hook);
   return 0;
}

template<class T>
static int registerBeforeAllHook() {
   HookRegistrar::addBeforeAllHook(boost::make_shared<T>());
   return 0;
}

template<class T>
static int registerAfterAllHook() {
   HookRegistrar::addAfterAllHook(boost::make_shared<T>());
   return 0;
}

}
}

#endif /* CUKE_HOOKREGISTRAR_HPP_ */
