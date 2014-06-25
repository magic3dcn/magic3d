#pragma once
 
namespace MagicMath
{
    class GradientDescent
    {
    public:
        GradientDescent();
        GradientDescent(int maxIterCount);
        ~GradientDescent();

        void Run();

    protected:
        virtual void CalInitValue() = 0;
        virtual void CalGradient() = 0;
        virtual bool IsStop() = 0;
        virtual void CalStepLength() = 0;
        virtual void UpdateResult() = 0;

    private:
        int mMaxIterCount;
    };

    
}
