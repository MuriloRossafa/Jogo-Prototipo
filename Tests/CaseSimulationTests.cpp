#include "../Source/OtherSide/Core/CaseSimulation.h"
#include <cassert>
#include <cmath>
#include <iostream>
using namespace OtherSide;
static void Solve(CaseSimulation& s) {
    assert(s.Collect(Evidence::Registry)==Result::Accepted);
    assert(s.Collect(Evidence::Photo)==Result::Accepted);
    assert(s.Collect(Evidence::Testimony)==Result::Accepted);
    assert(s.ChallengeWitness()==Result::Accepted);
    assert(s.Collect(Evidence::Recording)==Result::Accepted);
    assert(s.Conclude(Hypothesis::TemporalLoop)==Result::Accepted);
    assert(s.Collect(Evidence::Report)==Result::Accepted);
}
int main() {
    {
        CaseSimulation s;
        assert(s.Conclude(Hypothesis::TemporalLoop)==Result::MissingEvidence);
        assert(s.ChallengeWitness()==Result::MissingEvidence);
        assert(s.Collect(Evidence::Report)==Result::MissingEvidence);
        assert(s.Release()==Result::MissingEvidence);
        assert(s.Ritual(1)==Result::MissingEvidence);
        assert(s.Count()==0 && !s.DocumentRecovered);
        Solve(s); assert(s.Count()==5);
        assert(s.Collect(Evidence::Photo)==Result::Duplicate);
        assert(s.Conclude(Hypothesis::Fraud)==Result::Duplicate);
        assert(s.Ritual(1)==Result::Accepted);
        assert(s.Ritual(3)==Result::WrongSequence && s.RitualStep==0);
        assert(s.Ritual(1)==Result::Accepted);
        assert(s.Ritual(2)==Result::Accepted);
        assert(s.Outcome==Ending::None);
        assert(s.Ritual(3)==Result::Accepted && s.Outcome==Ending::Contained);
        const float b=s.Battery; s.Tick(100,true,false); s.Leave();
        assert(s.Battery==b && s.Outcome==Ending::Contained);
        assert(s.Release()==Result::Finished);
    }
    {
        CaseSimulation s;
        s.Collect(Evidence::Registry);s.Collect(Evidence::Photo);s.Collect(Evidence::Testimony);s.ChallengeWitness();
        assert(s.Conclude(Hypothesis::TemporalLoop)==Result::MissingEvidence);
        s.Collect(Evidence::Recording);
        assert(s.Conclude(Hypothesis::Fraud)==Result::WrongConclusion && !s.Conclusion);
        assert(s.Conclude(Hypothesis::TemporalLoop)==Result::Accepted);
    }
    { CaseSimulation s;Solve(s);assert(s.Release()==Result::Accepted);assert(s.Outcome==Ending::Released); }
    { CaseSimulation s;s.Leave();assert(s.Outcome==Ending::Escaped&&!s.DocumentRecovered); }
    { CaseSimulation s;Solve(s);s.Leave();assert(s.Outcome==Ending::Escaped&&s.DocumentRecovered); }
    {
        CaseSimulation s;s.Tick(2,true,false);assert(s.Exposure==2);
        s.Tick(1,false,true);assert(s.Exposure==0 && s.Fear==5);
        s.Tick(5,true,false);assert(s.Outcome==Ending::None);
        s.Tick(1,true,false);assert(s.Outcome==Ending::Lost);
    }
    {
        CaseSimulation s;s.ToggleLight();s.Tick(100,false,true);assert(s.Battery==100);
        s.ToggleLight();s.Tick(10000,false,true);assert(s.Battery==0&&!s.Flashlight);
        s.ToggleLight();assert(!s.Flashlight);
    }
    for(int scenario=0;scenario<100;++scenario) {
        CaseSimulation s;
        for(int i=0;i<2000&&s.Outcome==Ending::None;++i) {
            s.Tick(.016f,(i+scenario)%100<20,i%2==0);
            assert(std::isfinite(s.Fear)&&s.Fear>=0&&s.Fear<=100);
            assert(s.Battery>=0&&s.Battery<=100 && s.Exposure>=0);
        }
    }
    std::cout<<"PASS: evidence gates, contradiction, hypotheses, duplicate clues, ritual reset/order, containment, release, extraction with/without document, terminal states, pursuit recovery/defeat, battery and 100 resource scenarios.\n";
}
