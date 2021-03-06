# Getting Started with QCTools

*   [What is QCTools?](#qctools)
*   [Who can benefit from using QCTools?](#benefits)
*   [What video formats does QCTools accept?](#formats)
*   [What do the Graphs measure?](#measure)
*   [What Graphs should I chose?](#graphs)
*   [What is FFmpeg?](#ffmpeg)
*   [How can I integrate the QCTools export feature into my workflow?](#workflow)
*   [What about my audio?](#audio)
*   [What is this error called?](#error)
*   [Are there additional resources that may be useful to me?](#resources)
*   [How can I provide feedback?](#feedback)

## What is QCTools {#qctools}

QCTools (Quality Control Tools for Video Preservation) is a free and open source software tool that helps users analyze and understand their digitized video files through use of audiovisual analytics and filtering to help users detect corruptions or compromises in the results of analog video digitization or in born-digital video. The goal of the project is to cut down the time it takes to perform high-quality video preservation and direct time towards preservation issues that are solvable - for example, identifying tapes that would benefit from a second transfer, saving not only the precious time of preservationists and institutional resources, but giving collections a necessary advantage in the bigger race against time to preserve their significant cultural artifacts. QCTools incorporates archival standards and best practices for reformatting and capturing metadata that enables the long-term preservation of and access to the original artifact, the digital object, and the associated catalog record.

## Who can benefit from using QCTools (and how)? {#benefits}

Potential users of the quality control tools include specialists and non-specialists:

*   #### Archivists, librarians, and conservators.

    Moving image archives that collect digital video may receive digitized content with undiagnosed playback problems and errors. QCTools can help archivists understand what is wrong with certain files and then take steps to correct or explain these errors. Archivists need to be able to trust that the digitized files are up to a certain level of quality, and have quantifiable data to prove it. QCTools allows archivists to not be required to overly rely on visual queues and guesswork for the acknowledgment of problematic video quality.

    QCTools will benefit not only moving image collections but also traditional archives that include video collections that may fall into neglect. The rapid rate of deterioration makes the preservation of magnetic media-based archival elements of foremost import when deciding on preservation strategies; however, because the knowledge base is so specialized, many archivists lack a way to evaluate digitization measures. QCTools will make it easier and more approachable for these archives to preserve and maintain digitized moving image materials.

*   #### Media asset managers and media production coordinators

    Users working in this industry can use QCTools to perform quality checks on incoming media, as well as existing stored media. Work on large video collections can be done in-house and via spot-checking, instead of outsourcing much of the material. Furthermore, quality of video can be assured suitable for broadcast environments.

    For example, if a production company is working with editing footage, it is better to know early on if there is a problem with any of the video files, so that they can be retransferred or discarded from use.

*   #### Preservation technicians and moving image digitization specialists

    Preservation technicians and moving image digitization specialists need to maintain a high level of quality for all materials they are processing. QCTools assists with these projects. Instead of having to do a full visual check of video files to monitor for errors, technicians can use QCTools to identify problem areas for more efficient spot-checking. When integrated into a digitization workflow, QCTools can reduce time and increase the discovery of errors.

    For example, using QCTools regularly to verify audiovisual artifacts caused by VHS [Head Switching Noise](https://bavc.github.io/avaa/artifacts/head_switching_noise.html) or a U-Matic machine's [Video Head Clog](https://bavc.github.io/avaa/artifacts/video_head_clog.html) can prevent the need for additional transfers due to a machine that needs cleaning.

*   #### Artists and curators

    Artists digitizing their work, or having their work digitized for them, may not have the background knowledge necessary to understand why their videos look the way they do. QCTools can help artists interpret these errors so the necessary steps can be taken to keep digitized video looking as close to as they originally did.

*   #### Historians and collections managers

    Historians and collection managers that do not usually work with audiovisual historical documents may not know how to proceed with preserving video formats, and additionally not have the standard tools that may be found in archives equipped to handle audiovisual materials. QCTools can work in lieu of access to expensive proprietary software.

    For example, a small state historical society may have a collection of tapes, but because they are a small staff and cannot afford to outsource digitization, the tapes have been neglected. If the staff is able to get the equipment to digitize the tapes in-house, QCTools can help them diagnose issues and make the digitization process less intimidating while ensuring high quality digital files.

## What video formats does QCTools accept? {#formats}

QCTools accepts a variety of video formats, including \*.avi, \*.mkv, \*.mov, and \*.mp4, as well as a diverse selection of codecs. Uncompressed video formats are ideal for the most accurate interpretation of the video. QCTools relies on FFmpeg's libavcodec and libformat libraries to analyze and playback video which allows QCTools to support a wide variety of digital audiovisual formats.

## What do the Graphs measure? {#measure}

The following provides an overview of QCTools graph functionality. For more detailed descriptions, please see the help documentation on Filter Descriptions or the documentation on FFmpeg's [signalstats](http://ffmpeg.org/ffmpeg-filters.html#signalstats) filter.

### Y Channel Filters (Y, YDiff)

The Y Channel carries brightness information (also called luma). Black and white moving image files will contain only Y data whereas a colorized video with have both Y, and U/V. If you are analyzing a black and white video, then, you should see no U/V data. If you do, this may indicate that your video has color noise. These filters are ideal for determining if your video is overly bright, or dark. The Diff filters simply measure the visual difference in value between the Y values of two consecutive pixels.

### U/V Channel Filters (U, V, UDiff, VDiff)

These 2 channels together carry color information (or chroma); dramatic activity in these graphs could indicate that your video has color abnormalities. A normal reading indicating consistent color values would show graph values the around the central value of 128.

### Saturation

This graph detects the measure of vibrancy or brightness of color. Overly saturated (too bright) or under-saturated (too washed-out) are not ideal. Saturation levels that are too high may exceed broadcast range, extremely high levels are mathematically illegal as they can not be converted back to RGB without creating an invalid value, such values are generally indicative of tape damage.

### Hue

The Hue graphs plot the average color value for the frame. Do the blues, reds, etc., of your video look as you expect they should? This graph filter will help you detect areas of your video which may have strange color representation.

### Temporal Outliers

This filter looks at a collection of pixels that reside near to one another; in video with no errors, you would expect for these pixels to have roughly a similar value. If there are large differences between a pixel and it's neighbors, this may be an indication of a dropout or other error. This graph filter is especially good at detecting white speckling artifacts such as tape creases, tape tracking issues, or skew problems.

### Vertical Repetitions (VREP)

This graph filter works by looking for rows of similar or duplicate pixels. Often, if there is signal interruption or loss, your deck or Time Base Corrector, in an effort to fill in the gaps, will duplicate existing rows of pixel data. The VREP filter will alert you to this repetitive data, which often manifests as vertical lines across your picture. This filter is useful with analog source content only.

### Broadcast Range (BRNG)

This filter detects pixels which lie outside of [broadcast range](http://en.wikipedia.org/wiki/Broadcast-safe) or those deemed to have unacceptable values as per established broadcasting standards. Also see [Broadcast Standards](http://en.wikipedia.org/wiki/Broadcast_television_systems).

### Mean Square Error per Field (MSeF)

This detects the visual difference between the two fields of video. The bigger the difference, the more it indicates a potential problem (head clogs, deck issues). See FFmpeg's documentation on the[PSNR filter](http://ffmpeg.org/ffmpeg-filters.html#psnr) for more information.

### Peak Signal to Noise Ratio per Field (PSNRf)

This filter is similar to Mean Square Error per Field but uses the Peak Signal to Noise Ratio (PSNR) algorithm. See FFmpeg's documentation on the[PSNR filter](http://ffmpeg.org/ffmpeg-filters.html#psnr) for more information.

### Structural SImilarity Metric per Field (SSIMf)

This filter is similar to PSNRf and MSEf but uses the Structural SImilarity Metric (SSIM) algorithm. See FFmpeg's documentation on the[SSIM filter](http://ffmpeg.org/ffmpeg-filters.html#ssim) for more information.

## What Graphs should I chose? {#graphs}

The graphs detect a wide array of errors and as you become more familiar with the tool and your video collection, you will likely opt for different graph filters depending on the errors you suspect or anticipate. There are some multipurpose options, however, that may be especially helpful to start with in order to get a high-level sense of where, and what type of errors are present. The YUV and Diffs (Y values, U values, V values, YDiff, YDiffX) are beneficial for detecting light/dark and color issues. The TOUT (Temporal Outliers) filter is targeted to detecting white noise such as white speckle pattern you may find in damaged VHS tape. These graph filters are ideal to start with as a new user.

## What is FFmpeg? {#ffmpeg}

FFmpeg is a complete, cross-platform solution to record, convert and stream audio and video. QCTools utilizes FFmpeg to decode video for analysis. The playback filters use FFmpeg's libavfilter in order to process the decoded audiovisual data for different types of displays. More at [FFmpeg.org](ffmpeg.org).

## How can I integrate the QCTools export feature into my workflow? {#workflow}

QCTools currently supports a frame-by-frame analysis/export in XML format (based on the FFprobe metadata standard. Please also see the 'Data Format' tab). These files can be integrated into your preservation metadata.

Since the QCTool's analysis filter, [signalstats](http://ffmpeg.org/ffmpeg-filters.html#signalstats), is now part of FFmpeg and QCTool's reporting format is based on FFmpeg's ffprobe metadata schema, QCTools data reports may be fully generated using a recent version of FFmpeg. This may be preferable for handling the analysis of large amounts of audiovisual files, so that FFmpeg creates many QCTools reports through FFmpeg which can then later be assessed more closely within the QCTools application. See the Data Format section of the help documentation for more information.

## What about my audio? {#audio}

Starting in QCTools version 0.7, QCTools has some audio support. This includes plotting EBU R128 loudness measurements over time and providing several audio visualization playback tools (audio vectorscope, audio waveforms, etc) in the player window.

## What is this error called? {#error}

You may be able to find the answer in the [A/V Artifact Atlas](http://preservation.bavc.org/artifactatlas/index.php/A/V_Artifact_Atlas), which is closely associated with QCTools. The AV Artifact Atlas is for use in the identification and definition of the technical issues and anomalies that can afflict audio and video signals.

## Are there additional resources that may be useful to me? {#resources}

The [Glossary of Video Terms](http://en.wikipedia.org/wiki/Glossary_of_video_terms) is helpful for a general primer on common terms.

The [AV Artifact Atlas](http://preservation.bavc.org/artifactatlas/index.php/A/V_Artifact_Atlas) is an especially good resource to find examples of video errors, with accompanying descriptions and visual references.

## How can I provide feedback? {#feedback}

Feedback such as requests for clarifications, suggestions for enhancements, or reports of bugs is important for the project. If you find an issue with QCTools, find something unclear, or have an idea please visit the issue tracker at [QCTool's github account](http://github.com/bavc/qctools/issues) and click [New issue](http://github.com/bavc/qctools/issues/new). Once here please name your issue and fill out a description. Once submitted the issue will be in the public issue tracker and the development team can respond to it or associate improvements to it.
