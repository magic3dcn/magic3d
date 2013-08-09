#pragma once

namespace MagicCore
{
    class MagicFramework
    {
    public:
        MagicFramework();
        ~MagicFramework();

        void Init(void);
        void Run(void);

    private:
        void Update(float timeElapsed);
        bool Running(void);

    private:
        float mTimeAccumulate;
        float mRenderDeltaTime;
    };
}
