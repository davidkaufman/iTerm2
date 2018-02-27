//
//  iTermMetalDriverDataSource.h
//  iTerm2
//
//  Created by George Nachman on 2/18/18.
//

#import <Cocoa/Cocoa.h>

#import "iTermASCIITextRenderer.h"
#import "iTermColorComputer.h"
#import "iTermCursorType.h"
#import "iTermImageRenderer.h"
#import "iTermIndicatorRenderer.h"
#import "iTermMarkRenderer.h"
#import "iTermMetalGlyphKey.h"
#import "iTermTextRendererCommon.h"
#import "VT100GridTypes.h"

#import <simd/simd.h>

@class iTermData;

NS_ASSUME_NONNULL_BEGIN

NS_CLASS_AVAILABLE(10_11, NA)
@interface iTermMetalCursorInfo : NSObject
@property (nonatomic) BOOL cursorVisible;
@property (nonatomic) VT100GridCoord coord;
@property (nonatomic) ITermCursorType type;
@property (nonatomic, strong) NSColor *cursorColor;

// Block cursors care about drawing the character overtop the cursor in a
// different color than the character would normally be. If this is set, the
// text color will be changed to that of the `textColor` property.
@property (nonatomic) BOOL shouldDrawText;
@property (nonatomic) vector_float4 textColor;

// This is a "frame" cursor, as seen when the view does not have focus.
@property (nonatomic) BOOL frameOnly;
@property (nonatomic) BOOL copyMode;
@property (nonatomic) BOOL copyModeCursorSelecting;
@property (nonatomic) VT100GridCoord copyModeCursorCoord;
@end

@interface iTermMetalIMEInfo : NSObject

@property (nonatomic) VT100GridCoord cursorCoord;
@property (nonatomic) VT100GridCoordRange markedRange;

- (void)setRangeStart:(VT100GridCoord)start;
- (void)setRangeEnd:(VT100GridCoord)end;
- (NSRange)markedRangeOnLine:(int)y width:(int)width;

@end

NS_CLASS_AVAILABLE(10_11, NA)
@protocol iTermMetalDriverDataSourcePerFrameState<NSObject>

@property (nonatomic, readonly) VT100GridSize gridSize;
@property (nonatomic, readonly) CGSize cellSize;
@property (nonatomic, readonly) CGSize cellSizeWithoutSpacing;
@property (nonatomic, readonly) vector_float4 defaultBackgroundColor;
@property (nonatomic, readonly) vector_float4 processedDefaultBackgroundColor;
@property (nonatomic, readonly) NSImage *badgeImage;
@property (nonatomic, readonly) CGRect badgeSourceRect;
@property (nonatomic, readonly) CGRect badgeDestinationRect;
@property (nonatomic, nullable, readonly) iTermMetalIMEInfo *imeInfo;
@property (nonatomic, readonly) BOOL showBroadcastStripes;
@property (nonatomic, readonly) NSColor *cursorGuideColor;
@property (nonatomic, readonly) BOOL cursorGuideEnabled;
@property (nonatomic, readonly) vector_float4 fullScreenFlashColor;
@property (nonatomic, readonly) BOOL timestampsEnabled;
@property (nonatomic, readonly) NSColor *timestampsBackgroundColor;
@property (nonatomic, readonly) NSColor *timestampsTextColor;
@property (nonatomic, readonly) long long firstVisibleAbsoluteLineNumber;
@property (nonatomic, readonly) NSData *colorMap;
@property (nonatomic, readonly) iTermData *concatenatedLines;
@property (nonatomic, readonly) iTermData *selectedIndices;
@property (nonatomic, readonly) iTermData *findMatches;
@property (nonatomic, readonly) iTermData *annotatedIndices;
@property (nonatomic, readonly) iTermData *markedIndices;
@property (nonatomic, readonly) iTermData *underlinedIndices;

- (void)metalGetGlyphKeys:(iTermMetalGlyphKey *)glyphKeys
                imageRuns:(NSMutableArray<iTermMetalImageRun *> *)imageRuns
                markStyle:(out iTermMarkStyle *)markStylePtr
                      row:(int)row
                    width:(int)width
           drawableGlyphs:(int *)drawableGlyphsPtr
                     date:(out NSDate * _Nonnull * _Nonnull)date;

- (nullable iTermMetalCursorInfo *)metalDriverCursorInfo;

- (NSDictionary<NSNumber *, iTermCharacterBitmap *> *)metalImagesForGlyphKey:(iTermMetalGlyphKey *)glyphKey
                                                                        size:(CGSize)size
                                                                       scale:(CGFloat)scale
                                                                       emoji:(BOOL *)emoji;

// Returns the background image or nil. If there's a background image, fill in blending and tiled.
- (NSImage *)metalBackgroundImageGetTiled:(nullable BOOL *)tiled;

// An object that compares as equal if ascii characters produced by metalImagesForGlyph would
// produce the same bitmap.
- (id)metalASCIICreationIdentifier;

// Returns metrics and optional color for underlines.
- (void)metalGetUnderlineDescriptorsForASCII:(out iTermMetalUnderlineDescriptor *)ascii
                                    nonASCII:(out iTermMetalUnderlineDescriptor *)nonAscii;

- (void)enumerateIndicatorsInFrame:(NSRect)frame block:(void (^)(iTermIndicatorDescriptor *))block;

- (void)metalEnumerateHighlightedRows:(void (^)(vector_float3 color, NSTimeInterval age, int row))block;

- (iTermASCIIRow *)asciiRowAtIndex:(NSUInteger)index;
- (void)getConfiguration:(iTermColorsConfiguration *)configurationPtr;

@end

NS_CLASS_AVAILABLE(10_11, NA)
@protocol iTermMetalDriverDataSource<NSObject>

- (nullable id<iTermMetalDriverDataSourcePerFrameState>)metalDriverWillBeginDrawingFrame;

- (void)metalDriverDidDrawFrame:(id<iTermMetalDriverDataSourcePerFrameState>)perFrameState;

- (void)metalDidFindImages:(NSSet<NSString *> *)foundImages
             missingImages:(NSSet<NSString *> *)missingImages
             animatedLines:(NSSet<NSNumber *> *)animatedLines;  // absolute line numbers

- (void)metalDriverDidProduceDebugInfo:(NSData *)archive;

@end

NS_ASSUME_NONNULL_END
