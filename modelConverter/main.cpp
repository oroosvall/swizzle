
#include <utils/ArgParse.hpp>

#include "Mesh.hpp"
#include "Model.hpp"

#include <assert.h>
#include <utils/Bitstream.hpp>

#include <swizzle/Swizzle.hpp>

class StdLogger : public swizzle::core::LogDevice
{
    // Inherited via LogDevice
    virtual void logMessage(SwChar* messageType, SwChar* message) override
    {
        printf("%s: %s\n", messageType, message);
    }
};

int main(int argv, char* argc[])
{
    //swizzle::core::SwCoreInitialize();

    StdLogger logger;

    utils::ArgOption inFiles("-in", "input files", utils::ArgType::Value);
    utils::ArgOption outFiles("-out", "output file", utils::ArgType::Value);
    utils::ArgOption compress("-c", ".swm attempt model compression", utils::ArgType::Flag);
    utils::ArgOption verbose("-v", "Verbose", utils::ArgType::Flag);
    utils::ArgOption benchmark("-b", "Benchmark", utils::ArgType::Flag);
    // utils::ArgOption triangleIndexCompress("-cti", ".swm triangle index compression", utils::ArgType::Flag);
    utils::ArgOption version("-ver", "specify target swm version", utils::ArgType::Value);

    utils::ArgParser args("Model conversion utility");
    args.addOption(&inFiles);
    args.addOption(&outFiles);
    args.addOption(&compress);
    args.addOption(&verbose);
    args.addOption(&benchmark);
    // args.addOption(&vertexIndexCompress);
    // args.addOption(&triangleIndexCompress);
    args.addOption(&version);

    if (verbose.mIsSet)
    {
        swizzle::core::AddLogger(&logger);
    }


    if (!args.parse(argv, argc))
        args.printHelp();

    if (inFiles.mIsSet && outFiles.mIsSet)
    {
        // CompressionFlags compressionFlags;
        // compressionFlags.mVertexIndex = vertexIndexCompress.mIsSet;
        // compressionFlags.mTriangleIndex = triangleIndexCompress.mIsSet;

        Model model;
        model.load(inFiles.mValues[0]);
        model.save(outFiles.mValues[0], compress.mIsSet);
    }

    //swizzle::core::SwCoreDeinitialize();

    return 0;
}
