//#include "VulkanStagingBuffer.hpp"
//
//namespace renderer
//{
//
//    StagingBuffer* StagingBuffer::sInstange = nullptr;
//
//    StagingBuffer* StagingBuffer::getStagingBuffer()
//    {
//        if (sInstange == nullptr)
//        {
//            sInstange = new StagingBuffer();
//        }
//        return sInstange;
//    }
//    void StagingBuffer::release()
//    {
//        delete sInstange;
//        sInstange = nullptr;
//    }
//
//    void StagingBuffer::beginStaging()
//    {
//
//        mCommandBuffer->begin();
//    }
//
//    void StagingBuffer::endStaging()
//    {
//        mCommandBuffer->end();
//    }
//
//    void StagingBuffer::setCommandBuffer(engine::Ref<VulkanCommandBuffer> cmdBuffer)
//    {
//        mCommandBuffer = cmdBuffer;
//    }
//
//	engine::Ref<VulkanCommandBuffer> StagingBuffer::getCommandBuffer() const
//    {
//        return mCommandBuffer;
//    }
//
//} // namespace renderer