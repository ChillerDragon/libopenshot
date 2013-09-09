#ifndef OPENSHOT_TIMELINE_H
#define OPENSHOT_TIMELINE_H

/**
 * \file
 * \brief Header file for Timeline class
 * \author Copyright (c) 2008-2013 OpenShot Studios, LLC
 */

// Defining some ImageMagic flags... for Mac (since they appear to be unset)
#ifndef MAGICKCORE_QUANTUM_DEPTH
	#define MAGICKCORE_QUANTUM_DEPTH 16
#endif
#ifndef MAGICKCORE_HDRI_ENABLE
	#define MAGICKCORE_HDRI_ENABLE 0
#endif

#include <list>
#include <omp.h>
#include <tr1/memory>
#include "Magick++.h"
#include "Cache.h"
#include "Color.h"
#include "Clip.h"
#include "ReaderBase.h"
#include "Fraction.h"
#include "Frame.h"
#include "FrameRate.h"
#include "KeyFrame.h"

using namespace std;
using namespace openshot;

namespace openshot {

	/// Comparison method for sorting clip pointers (by Position and Layer)
	struct compare_clip_pointers{
		bool operator()( Clip* lhs, Clip* rhs){
		return lhs->Position() <= rhs->Position() && lhs->Layer() < rhs->Layer();
	}};

	/**
	 * \brief This class represents a timeline
	 *
	 * The timeline is one of the most important features of a video editor, and controls all
	 * aspects of how video, image, and audio clips are combined together, and how the final
	 * video output will be rendered.  It has a collection of layers and clips, that arrange,
	 * sequence, and generate the final video output.
	 */
	class Timeline : public ReaderBase {
	private:
		int width; ///<Width of the canvas and viewport
		int height; ///<Height of the canvas and viewport
		Framerate fps; ///<Frames per second of the timeline
		int sample_rate; ///<Sample rate of timeline
		int channels; ///<Channels in timeline
		list<Clip*> clips; ///<List of clips on this timeline
		list<Clip*> closing_clips; ///<List of clips that need to be closed
		map<Clip*, Clip*> open_clips; ///<List of 'opened' clips on this timeline
		Cache final_cache; ///<Final cache of timeline frames

		/// Process a new layer of video or audio
		void add_layer(tr1::shared_ptr<Frame> new_frame, Clip* source_clip, int clip_frame_number, int timeline_frame_number);

		/// Calculate time of a frame number, based on a framerate
		float calculate_time(int number, Framerate rate);

		/// Calculate the # of samples per video frame (for a specific frame number)
		int GetSamplesPerFrame(int frame_number);

		/// Compare 2 floating point numbers for equality
		bool isEqual(double a, double b);

		/// Update the list of 'opened' clips
		void update_open_clips(Clip *clip, bool is_open);

		/// Update the list of 'closed' clips
		void update_closed_clips();

	public:

		/// Default Constructor for the timeline (which sets the canvas width and height and FPS)
		Timeline(int width, int height, Framerate fps, int sample_rate, int channels);

		/// Add an openshot::Clip to the timeline
		void AddClip(Clip* clip);

		/// Remove an openshot::Clip to the timeline
		void RemoveClip(Clip* clip);

		/// Close the reader (and any resources it was consuming)
		void Close();

		/// Get the framerate of this timeline
		Framerate FrameRate() { return fps; };

		/// Set the framerate for this timeline
		void FrameRate(Framerate new_fps) { fps = new_fps; };

		/// Get an openshot::Frame object for a specific frame number of this timeline.
		///
		/// @returns The requested frame (containing the image)
		/// @param[requested_frame] number The frame number that is requested.
		tr1::shared_ptr<Frame> GetFrame(int requested_frame) throw(ReaderClosed);

		/// Get the height of canvas and viewport
		int Height() { return height; }

		/// Set the height of canvas and viewport
		void Height(int new_height) { height = new_height; }

		// Curves for the viewport
		Keyframe viewport_scale; ///<Curve representing the scale of the viewport (0 to 100)
		Keyframe viewport_x; ///<Curve representing the x coordinate for the viewport
		Keyframe viewport_y; ///<Curve representing the y coordinate for the viewport

		// Background color
		Color color; ///<Background color of timeline canvas

		/// Open the reader (and start consuming resources)
		void Open();

		/// Sort clips by position on the timeline
		void SortClips();

		/// Get the width of canvas and viewport
		int Width() { return width; }

		/// Set the width of canvas and viewport
		void Width(int new_width) { width = new_width; }

	};


}

#endif
