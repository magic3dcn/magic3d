#pragma once

namespace MagicApp
{
    class MediaStream
    {
    public:
        MediaStream();
        ~MediaStream();

    public:
        void Init();
        void StartScanner();
        void StopScanner();
        void StartRecorder();
        void StopRecord();
        void StartRecordScanner();
        void StopRecordScanner();

    };


}