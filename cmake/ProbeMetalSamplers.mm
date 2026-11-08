#import <Foundation/Foundation.h>
#import <Metal/Metal.h>

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc != 5 && argc != 6) return 2;
    const unsigned cubes = atoi(argv[1]), sampled = atoi(argv[2]);
    const bool mixed = atoi(argv[3]) != 0, alias = atoi(argv[4]) != 0;
    if ((cubes != 4 && cubes != 8) || !sampled || sampled > cubes) return 2;
    @autoreleasepool {
        id<MTLDevice> device = MTLCreateSystemDefaultDevice();
        if (!device) return 3;
        const unsigned twoD = mixed ? (cubes == 4 ? 8 : 4) : 0;
        const unsigned count = mixed ? 16 : cubes;
        NSMutableString *source = [NSMutableString stringWithString:
            @"#include <metal_stdlib>\n"
             "vertex metal::float4 vs(uint i [[vertex_id]]) { return metal::float4("
             "i==1?3.0:-1.0,i==2?3.0:-1.0,0.5,1.0); }\n"
             "fragment metal::float4 fs(constant metal::uint2& cfg [[buffer(0)]],"];
        for (unsigned i = 0; i < count; ++i) {
            NSString *type = i < twoD ? @"texture2d" : i < twoD + cubes ? @"texturecube" : @"texture3d";
            [source appendFormat:@"metal::%@<float> t%u [[texture(%u)]],", type, i, i];
        }
        for (unsigned i = 0; i < count; ++i)
            [source appendFormat:@"metal::sampler s%u [[sampler(%u)]]%@", i, i, i + 1 == count ? @") {" : @","];
        [source appendString:@"metal::float4 c=0.0;"];
        for (unsigned i = 0; i < count; ++i) {
            const unsigned kind = i < twoD ? 0 : i < twoD + cubes ? 1 : 2;
            const unsigned ordinal = kind == 0 ? i : kind == 1 ? i - twoD : i - twoD - cubes;
            NSString *at = kind == 0 ? @"metal::float2(0.5)" : kind == 1 ? @"metal::float3(1,0,0)" : @"metal::float3(0.5)";
            [source appendFormat:@"if(cfg.y==%u && cfg.x>%u) c+=t%u.sample(s%u,%@);", kind, ordinal, i, i, at];
        }
        [source appendString:@"return c; }"];
        NSError *error = nil;
        if (argc == 6) {
            source = [[NSString stringWithContentsOfFile:[NSString stringWithUTF8String:argv[5]]
                encoding:NSUTF8StringEncoding error:&error] mutableCopy];
            if (!source) { fprintf(stderr, "%s\n", error.description.UTF8String); return 4; }
        }
        id<MTLLibrary> library = [device newLibraryWithSource:source options:nil error:&error];
        if (!library) { fprintf(stderr, "%s\n", error.description.UTF8String); return 4; }
        MTLRenderPipelineDescriptor *pipelineDesc = [MTLRenderPipelineDescriptor new];
        pipelineDesc.vertexFunction = [library newFunctionWithName:@"vs"];
        pipelineDesc.fragmentFunction = [library newFunctionWithName:argc == 6 ? @"main0" : @"fs"];
        pipelineDesc.colorAttachments[0].pixelFormat = MTLPixelFormatRGBA8Unorm;
        id<MTLRenderPipelineState> pipeline = [device newRenderPipelineStateWithDescriptor:pipelineDesc error:&error];
        if (!pipeline) { fprintf(stderr, "%s\n", error.description.UTF8String); return 5; }
        MTLSamplerDescriptor *samplerDesc = [MTLSamplerDescriptor new];
        samplerDesc.minFilter = samplerDesc.magFilter = MTLSamplerMinMagFilterNearest;
        samplerDesc.sAddressMode = samplerDesc.tAddressMode = samplerDesc.rAddressMode = MTLSamplerAddressModeClampToEdge;
        id<MTLSamplerState> sampler = [device newSamplerStateWithDescriptor:samplerDesc];
        id<MTLTexture> textures[16] = {};
        unsigned char texels[32];
        for (unsigned i = 0; i < sizeof(texels); ++i) texels[i] = i % 4 == 3 ? 255 : 32;
        for (unsigned i = 0; i < count; ++i) {
            if (alias && i > twoD && i < twoD + cubes) {
                textures[i] = textures[twoD];
                continue;
            }
            MTLTextureDescriptor *desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:
                MTLPixelFormatRGBA8Unorm width:2 height:2 mipmapped:NO];
            const bool cube = i >= twoD && i < twoD + cubes;
            const bool volume = i >= twoD + cubes;
            desc.textureType = cube ? MTLTextureTypeCube : volume ? MTLTextureType3D : MTLTextureType2D;
            desc.depth = volume ? 2 : 1;
            desc.storageMode = MTLStorageModeShared;
            desc.usage = MTLTextureUsageShaderRead;
            textures[i] = [device newTextureWithDescriptor:desc];
            if (!textures[i]) return 6;
            for (unsigned face = 0; face < (cube ? 6u : 1u); ++face)
                [textures[i] replaceRegion:MTLRegionMake3D(0,0,0,2,2,volume ? 2 : 1)
                    mipmapLevel:0 slice:face withBytes:texels bytesPerRow:8 bytesPerImage:16];
        }
        MTLTextureDescriptor *outputDesc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:
            MTLPixelFormatRGBA8Unorm width:4 height:4 mipmapped:NO];
        outputDesc.storageMode = MTLStorageModeShared;
        outputDesc.usage = MTLTextureUsageRenderTarget;
        id<MTLTexture> output = [device newTextureWithDescriptor:outputDesc];
        if (!output) return 7;
        id<MTLCommandQueue> queue = [device newCommandQueue];
        id<MTLCommandBuffer> commands = [queue commandBuffer];
        MTLRenderPassDescriptor *pass = [MTLRenderPassDescriptor renderPassDescriptor];
        pass.colorAttachments[0].texture = output;
        pass.colorAttachments[0].loadAction = MTLLoadActionClear;
        pass.colorAttachments[0].storeAction = MTLStoreActionStore;
        pass.colorAttachments[0].clearColor = MTLClearColorMake(0,0,0,1);
        id<MTLRenderCommandEncoder> encoder = [commands renderCommandEncoderWithDescriptor:pass];
        [encoder setRenderPipelineState:pipeline];
        [encoder setViewport:(MTLViewport){0,0,4,4,0,1}];
        const unsigned config[2] = {sampled, 1};
        if (argc == 6) {
            float draw[20][4] = {};
            float stage[69][4] = {};
            unsigned ordinals = 0;
            for (unsigned i = 0; i < 8; ++i) {
                // CKFragment1: 16 bump rows, 16 stage rows, 5 program rows,
                // then 16 border colors and 16 sampler descriptions.
                stage[16 + i * 2][1] = 3;
                stage[16 + i * 2][2] = 1;
                stage[32 + i / 2][(i % 2) * 2] = float((1 << 21) |
                    (i ? (7 | (1 << 5) | (2 << 10)) : (2 | (2 << 5))));
                stage[32 + i / 2][(i % 2) * 2 + 1] = float(2 | (1 << 5));
                ordinals |= i << (i * 3);
            }
            stage[36][0] = float(sampled - 1);
            stage[36][1] = float(ordinals);
            for (unsigned i = 0; i < 16; ++i) stage[53 + i][1] = stage[53 + i][2] = 1;
            [encoder setFragmentBytes:draw length:sizeof(draw) atIndex:0];
            [encoder setFragmentBytes:stage length:sizeof(stage) atIndex:1];
        } else {
            [encoder setFragmentBytes:config length:sizeof(config) atIndex:0];
        }
        for (unsigned i = 0; i < count; ++i) {
            [encoder setFragmentTexture:textures[i] atIndex:i];
            [encoder setFragmentSamplerState:sampler atIndex:i];
        }
        [encoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:3];
        [encoder endEncoding];
        [commands commit];
        [commands waitUntilCompleted];
        if (commands.status == MTLCommandBufferStatusError) {
            fprintf(stderr, "%s\n", commands.error.description.UTF8String);
            return 8;
        }
        unsigned char pixels[64];
        [output getBytes:pixels bytesPerRow:16 fromRegion:MTLRegionMake2D(0,0,4,4) mipmapLevel:0];
        const unsigned char *pixel = pixels + 40;
        const unsigned expected = sampled == 8 ? 255 : sampled * 32;
        printf("Metal cube samplers: declared=%u sampled=%u mixed=%d alias=%d RGBA=%u,%u,%u,%u expected=%u\n",
               cubes, sampled, mixed, alias, pixel[0], pixel[1], pixel[2], pixel[3], expected);
        return pixel[0] == expected && pixel[1] == expected && pixel[2] == expected && pixel[3] == 255 ? 0 : 9;
    }
}
