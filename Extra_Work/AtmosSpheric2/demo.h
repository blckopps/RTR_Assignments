

#include<GL/glew.h>
#include<gl/GL.h>

#include <memory>

#include "model.h"

namespace atmosphere {
	namespace demo {

		class Demo {
		public:
			Demo(int viewport_width, int viewport_height);
			~Demo();

			const Model& model() const { return *model_; }
			const GLuint vertex_shader() const { return vertex_shader_; }
			const GLuint fragment_shader() const { return fragment_shader_; }
			const GLuint program() const { return program_; }

		private:
			enum Luminance {
				// Render the spectral radiance at kLambdaR, kLambdaG, kLambdaB.
				NONE,
				// Render the sRGB luminance, using an approximate (on the fly) conversion
				// from 3 spectral radiance values only (see section 14.3 in <a href=
				// "https://arxiv.org/pdf/1612.04336.pdf">A Qualitative and Quantitative
				//  Evaluation of 8 Clear Sky Models</a>).
				APPROXIMATE,
				// Render the sRGB luminance, precomputed from 15 spectral radiance values
				// (see section 4.4 in <a href=
				// "http://www.oskee.wz.cz/stranka/uploads/SCCG10ElekKmoch.pdf">Real-time
				//  Spectral Scattering in Large-scale Natural Participating Media</a>).
				PRECOMPUTED
			};

			void InitModel();
			void HandleRedisplayEvent() const;
			void HandleReshapeEvent(int viewport_width, int viewport_height);
			void HandleKeyboardEvent(unsigned char key);
			void HandleMouseClickEvent(int button, int state, int mouse_x, int mouse_y);
			void HandleMouseDragEvent(int mouse_x, int mouse_y);
			void HandleMouseWheelEvent(int mouse_wheel_direction);
			void SetView(double view_distance_meters, double view_zenith_angle_radians,
				double view_azimuth_angle_radians, double sun_zenith_angle_radians,
				double sun_azimuth_angle_radians, double exposure);

			bool use_constant_solar_spectrum_;
			bool use_ozone_;
			bool use_combined_textures_;
			bool use_half_precision_;
			Luminance use_luminance_;
			bool do_white_balance_;
			bool show_help_;

			std::unique_ptr<Model> model_;
			GLuint vertex_shader_;
			GLuint fragment_shader_;
			GLuint program_;
			GLuint full_screen_quad_vao_;
			GLuint full_screen_quad_vbo_;
			//std::unique_ptr<TextRenderer> text_renderer_;
			int window_id_;

			double view_distance_meters_;
			double view_zenith_angle_radians_;
			double view_azimuth_angle_radians_;
			double sun_zenith_angle_radians_;
			double sun_azimuth_angle_radians_;
			double exposure_;

			int previous_mouse_x_;
			int previous_mouse_y_;
			bool is_ctrl_key_pressed_;
		};

	}  // namespace demo
}  // namespace atmosphere

