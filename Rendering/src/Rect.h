namespace GLib {
	struct Rect
	{
		Rect() {}
		Rect(float l, float r, float t, float b) {
			left = l;
			right = r;
			top = t;
			bottom = b;
		}

		float Width() {
			return right - left;
		}

		float Height() {
			return bottom - top;
		}

		float left;
		float right;
		float top;
		float bottom;
	};
}