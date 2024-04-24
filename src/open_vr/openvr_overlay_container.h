#ifndef OPENVR_OVERLAY_CONTAINER_H
#define OPENVR_OVERLAY_CONTAINER_H

#include "openvr_data.h"
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/sub_viewport_container.hpp>
#include <godot_cpp/variant/transform3d.hpp>

namespace godot {
class OpenVROverlayContainer : public SubViewportContainer {
	GDCLASS(OpenVROverlayContainer, SubViewportContainer)

	friend class openvr_data;

public:
	enum TrackedDevice {
		None,
		HMD,
		LeftHand,
		RightHand,
		Custom
	};

	// If TrackedDevice is not None and the requested device is not available, the
	// fallback behavior decides what happens to the overlay.
	enum TrackedDeviceFallbackBehavior {
		Absolute, // Revert to absolute transform (positioned in the world)
		Hide // Hide the overlay
	};

private:
	openvr_data *ovr;
	vr::VROverlayHandle_t overlay;
	int overlay_id;

	float overlay_width_in_meters;
	bool overlay_visible;

	// custom_tracked_device_name is only used when tracked_device == Custom, the return
	// value of get_tracked_device_name will be overridden in other cases.
	TrackedDevice tracked_device;
	StringName custom_tracked_device_name;
	TrackedDeviceFallbackBehavior fallback_behavior;

	Transform3D absolute_position; // Used when tracked_device == None
	Transform3D tracked_device_relative_position; // Used when tracked_device != None

	void on_frame_post_draw();
	void draw_overlay(const Ref<Texture2D> &p_texture);

	bool update_overlay_transform();

	// Sent along by openvr_data when an event arrives for our overlay.
	void process_event(vr::VREvent_t event);

protected:
	static void _bind_methods();

public:
	OpenVROverlayContainer();
	~OpenVROverlayContainer();

	virtual void _ready() override;
	virtual void _exit_tree() override;

	float get_overlay_width_in_meters();
	void set_overlay_width_in_meters(float p_new_size);

	bool is_overlay_visible();
	void set_overlay_visible(bool p_visible);

	TrackedDevice get_tracked_device();
	void set_tracked_device(TrackedDevice p_tracked_device);

	StringName get_tracked_device_name();
	void set_tracked_device_name(StringName p_tracked_device_name);

	Transform3D get_absolute_position();
	void set_absolute_position(Transform3D p_position);

	Transform3D get_tracked_device_relative_position();
	void set_tracked_device_relative_position(Transform3D p_position);
};
} // namespace godot

VARIANT_ENUM_CAST(OpenVROverlayContainer::TrackedDevice);

#endif /* !OPENVR_OVERLAY_CONTAINER_H */
